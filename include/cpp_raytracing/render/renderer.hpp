/**
 * @file
 * @brief renderer
 */

#ifndef CPP_RAYTRACING_RENDERER_HPP
#define CPP_RAYTRACING_RENDERER_HPP

#include <cmath>
#include <functional>
#include <memory>
#include <omp.h>

#include "../geometry/base.hpp"
#include "../util.hpp"
#include "../values/color.hpp"
#include "../values/tensor.hpp"
#include "../world/entities/base.hpp"
#include "../world/materials/base.hpp"
#include "../world/ray_segment.hpp"
#include "../world/scene.hpp"
#include "canvas.hpp"
#include "image.hpp"

namespace cpp_raytracing {

/**
 * @brief image renderer
 */
template <Dimension DIMENSION>
class Renderer {

  public:
    /**
     * @brief callback parameter type representing the current state of the
     *        render process
     * @see RenderCallbackFunc, frequent_render_callback,
     *      infrequent_render_callback
     */
    struct State {
        /** @brief image */
        RawImage& image;
        /** @brief samples */
        unsigned long samples;
    };
    /** @brief callback type used by Renderer::render() */
    using RenderCallbackFunc = std::function<void(const State&)>;

    /**
     * @brief default minimal ray length
     * @see minimal_ray_length
     */
    constexpr static Scalar DEFAULT_RAY_MINIMAL_LENGTH = 1e-5;

    /**
     * @brief default infrequent_render_callback
     * @see infrequent_render_callback_frequency
     */
    constexpr static unsigned long DEFAULT_INFREQUENT_CALLBACK_FREQUENCY = 10;

    /** @brief canvas for the image to be rendered */
    Canvas canvas;

    /**
     * @brief amount of samples per pixels
     * @note 'The higher the better.'
     *       Typical values are:
     *       - 1...10 for previews (major noise)
     *       - 10...1000 for mid quality (some noise)
     *       - 1000...10000 for high quality (minor to no noise)
     *
     *       But this **heavily depends on the complexity of the scene!**
     */
    unsigned long samples = 1;
    /**
     * @brief max amount of ray segments to be simulated
     * @note A ray might be divieded into segments to simulate curved space.
     *       Each scattering creates a new, secondary ray.
     * @note There is no distinction between different types of ray segments.
     * @note The ray depth influences the maximal ray length in curved space.
     *       Therefore not all entities may be visible if it is too small.
     */
    unsigned long ray_depth = 1;

    /**
     * @brief color indicator for ray ended before hitting anything
     * @note May be used for global illumination or debugging.
     */
    Color ray_color_if_ray_ended = {0.0, 100.0, 0.0};
    /**
     * @brief fallback color for missing background
     * @note May be used for global illumination.
     */
    Color ray_color_if_no_background = {1.0, 1.0, 1.0};
    /** @brief color indicator for missing material */
    Color ray_color_if_no_material = {1.0, 0.0, 1.0};

    /**
     * @brief callback function to be called frequently during rendering e.g.
     *        to inform about the progress of rendering
     * @note set to `nullptr` if no callback is desired
     * @note Typically called after a full image sample.
     */
    RenderCallbackFunc frequent_render_callback = nullptr;
    /**
     * @brief callback function to be called infrequently during rendering
     *        rendering e.g. to save the current progress
     * @note set to `nullptr` if no callback is desired
     * @see infrequent_render_callback
     */
    RenderCallbackFunc infrequent_render_callback = nullptr;
    /**
     * @brief call infrequent_render_callback every
     *        `infrequent_render_callback` steps
     * @note A step might be a full image sample.
     */
    unsigned long infrequent_callback_frequency =
        DEFAULT_INFREQUENT_CALLBACK_FREQUENCY;

    /** @brief time the exposure of the frame starts */
    Scalar time = 0.0;

    /**
     * @brief minimal ray length
     * @note Must be strictly larger than zero.
     * @see maximal_ray_length
     */
    Scalar minimal_ray_length = DEFAULT_RAY_MINIMAL_LENGTH;

    /**
     * @brief if true color entitites based on normals instead of thier
     * materials
     * @see ray_color_if_exterior_normal, ray_color_if_interior_normal
     */
    bool debug_normals = false;

    /**
     * @brief color if ray hits a surface from the 'outside'
     * @see debug_normals
     */
    Color ray_color_if_exterior_normal = {0.0, 0.0, 1.0};
    /**
     * @brief color if ray hits a surface from the 'inside'
     * @see debug_normals
     */
    Color ray_color_if_interior_normal = {1.0, 0.0, 0.0};

    /** @brief default constructor */
    Renderer() = default;

    /** @brief copy constructor */
    Renderer(const Renderer&) = default;

    /** @brief move constructor */
    Renderer(Renderer&&) = default;

    /** @brief copy assignment */
    Renderer& operator=(const Renderer&) = default;

    /** @brief move assignment */
    Renderer& operator=(Renderer&&) = default;

    virtual ~Renderer() = default;

    /** @brief render Scene as RawImage */
    virtual RawImage render(const Geometry<DIMENSION>& geometry,
                            Scene<DIMENSION>& scene) = 0;

    /** @brief calculates color of light ray */
    Color ray_color(const Geometry<DIMENSION>& geometry,
                    const typename Scene<DIMENSION>::FreezeGuard& frozen_scene,
                    Ray<DIMENSION>* ray, const unsigned long depth) const {

        using namespace tensor;

        // propagate ray
        const std::optional<RaySegment<DIMENSION>> current_opt_segment =
            ray->next_ray_segment();

        // extract next segment
        if (!current_opt_segment.has_value()) {
            // ray ended (e.g. boundary of space or technical limit)
            return ray_color_if_ray_ended;
        }
        // has next segment
        const RaySegment<DIMENSION>& current_segment =
            current_opt_segment.value();

        // check depth limit
        if (depth == 0) {
            return background_color(geometry, frozen_scene, current_segment);
        }

        // collect hit record
        HitRecord<DIMENSION> record = frozen_scene.hit_record(
            geometry, current_segment, minimal_ray_length);

        // check for hit with entity within current segment
        if (!record.hits()) {
            if (current_segment.is_infinite()) {
                // no hit within entire ray
                // no further segments expected
                return background_color(geometry, frozen_scene,
                                        current_segment);
            }
            // no hit within current (finite) segment
            // further segments expected
            return ray_color(geometry, frozen_scene, ray, depth - 1);
        }
        // detected hit with entity within cuurent segment

        // debug mode for surface normals
        // note: no scattering for this mode
        if (debug_normals) {
            return record.front_face ? ray_color_if_exterior_normal
                                     : ray_color_if_interior_normal;
        }

        // check for material
        if (!record.material) {
            // no material
            return ray_color_if_no_material;
        }
        // has material

        // cast secondary ray or emitter
        const Mat3x3 to_onb_jacobian = geometry.to_onb_jacobian(record.point);

        const Vec3 onb_ray_direction =
            to_onb_jacobian * current_segment.direction();
        const auto [onb_scatter_direction, color] =
            record.material->scatter(record, onb_ray_direction);
        if (is_zero(onb_scatter_direction)) {
            // is emitter
            return color;
        } else {
            // cast secondary ray
            const Mat3x3 from_onb_jacobian =
                geometry.from_onb_jacobian(record.point);
            const Vec3 scattered_direction =
                from_onb_jacobian * onb_scatter_direction;
            std::unique_ptr<Ray<DIMENSION>> scattered_ray =
                geometry.ray_from(record.point, scattered_direction);

            return color * ray_color(geometry, frozen_scene,
                                     scattered_ray.get(), depth - 1);
        }
    }

  protected:
    /** @brief returns background color for ray segment */
    inline Color
    background_color(const Geometry<DIMENSION>& geometry,
                     const typename Scene<DIMENSION>::FreezeGuard& frozen_scene,
                     const RaySegment<DIMENSION>& ray_segment) const {
        if (frozen_scene.active_background == nullptr) {
            return ray_color_if_no_background;
        }
        return frozen_scene.active_background->value(geometry, ray_segment);
    }

    /** @brief render a single sample for a single pixel */
    inline void
    render_pixel_sample(const unsigned long i, const unsigned long j,
                        const Geometry<DIMENSION>& geometry,
                        const typename Scene<DIMENSION>::FreezeGuard& frozen_scene,
                        RawImage& buffer) const {
        // random sub-pixel offset for antialiasing
        Scalar x = Scalar(i) + random_scalar(-0.5, +0.5);
        Scalar y = Scalar(j) + random_scalar(-0.5, +0.5);
        // transform to camera coordinates
        x = (2.0 * x / static_cast<Scalar>(canvas.width) - 1.0);
        y = (2.0 * y / static_cast<Scalar>(canvas.height) - 1.0);

        std::unique_ptr<Ray<DIMENSION>> ray =
            frozen_scene.active_camera.ray_for_coords(geometry, x, y);
        const Color pixel_color =
            ray_color(geometry, frozen_scene, ray.get(), ray_depth);
        buffer[{i, j}] += pixel_color;
    }
};

/** @brief render interface for 3D manifolds */
using Renderer3D = Renderer<Dimension{3}>;

/** @brief renderer with global shutter and motion blur */
template <Dimension DIMENSION>
class GlobalShutterRenderer : public Renderer<DIMENSION> {
  public:
    /**
     * @brief total duration of the frame's exposure
     * @note `0.0` means no motion blur
     * @note should be smaller than the distance of two frames
     *        to be realistic
     */
    Scalar exposure_time = 0.0;

    /** @brief default constructor */
    GlobalShutterRenderer() = default;

    /** @brief copy constructor */
    GlobalShutterRenderer(const GlobalShutterRenderer&) = default;

    /** @brief move constructor */
    GlobalShutterRenderer(GlobalShutterRenderer&&) = default;

    /** @brief copy assignment */
    GlobalShutterRenderer& operator=(const GlobalShutterRenderer&) = default;

    /** @brief move assignment */
    GlobalShutterRenderer& operator=(GlobalShutterRenderer&&) = default;

    ~GlobalShutterRenderer() override = default;

    RawImage render(const Geometry<DIMENSION>& geometry,
                    Scene<DIMENSION>& scene) override {

        RawImage buffer{this->canvas.width, this->canvas.height};

        // optimization: reduce cache generation if possible
        if (this->exposure_time == 0.0) {
            // note: ideal image: no motion blur -> no nned for repeated
            //       temporal updates
            const typename Scene<DIMENSION>::FreezeGuard& frozen_scene =
                scene.freeze_for_time(random_scalar(
                    this->time, this->time + this->exposure_time));

            for (unsigned long s = 1; s < this->samples + 1; ++s) {
                render_sample(s, buffer, geometry, frozen_scene);
            }

        } else {
            // with motion blur
            for (unsigned long s = 1; s < this->samples + 1; ++s) {

                // scene for random time in exposure window
                const typename Scene<DIMENSION>::FreezeGuard& frozen_scene =
                    scene.freeze_for_time(random_scalar(
                        this->time, this->time + this->exposure_time));

                this->render_sample(s, buffer, geometry, frozen_scene);
            }
        }

        buffer *= 1 / (Scalar(this->samples));
        return buffer;
    }

  private:
    /** @brief render with global shutter and motion blur */
    inline void
    render_sample(const unsigned long sample, RawImage& buffer,
                  const Geometry<DIMENSION>& geometry,
                  const typename Scene<DIMENSION>::FreezeGuard& frozen_scene) {

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
        for (unsigned long j = 0; j < this->canvas.height; ++j) {
            for (unsigned long i = 0; i < this->canvas.width; ++i) {
                this->render_pixel_sample(i, j, geometry, frozen_scene, buffer);
            }
        }

        if (this->frequent_render_callback) {
            this->frequent_render_callback(
                typename Renderer<DIMENSION>::State{buffer, sample});
        }
        if (this->infrequent_render_callback &&
            (sample % this->infrequent_callback_frequency == 0)) {
            this->infrequent_render_callback(
                typename Renderer<DIMENSION>::State{buffer, sample});
        }
    }
};

/** @brief global shutter render for 3D manifolds */
using GlobalShutterRenderer3D = GlobalShutterRenderer<Dimension{3}>;

/** @brief renderer with rolling shutter and motion blur */
template <Dimension DIMENSION>
class RollingShutterRenderer : public Renderer<DIMENSION> {
  public:
    /**
     * @brief total duration of the frame's exposure
     * @note `0.0` means instant image
     * @note should be larger than zero to be realistic
     * @note should be smaller than the distance of two frames
     *       to be realistic
     */
    Scalar frame_exposure_time = 0.0;

    /**
     * @brief exposure time per line scanned times total number of lines
     * @note `0.0` means no motion blur
     * @note should be (much) smaller than the distance of two frames
     *       to be realistic
     */
    Scalar total_line_exposure_time = 0.0;

    /** @brief default constructor */
    RollingShutterRenderer() = default;

    /** @brief copy constructor */
    RollingShutterRenderer(const RollingShutterRenderer&) = default;

    /** @brief move constructor */
    RollingShutterRenderer(RollingShutterRenderer&&) = default;

    /** @brief copy assignment */
    RollingShutterRenderer& operator=(const RollingShutterRenderer&) = default;

    /** @brief move assignment */
    RollingShutterRenderer& operator=(RollingShutterRenderer&&) = default;

    ~RollingShutterRenderer() override = default;
    RawImage render(const Geometry<DIMENSION>& geometry,
                    Scene<DIMENSION>& scene) override {

        RawImage buffer{this->canvas.width, this->canvas.height};

        for (unsigned long s = 1; s < this->samples + 1; ++s) {
            this->render_sample(s, buffer, geometry, scene);
        }

        buffer *= 1 / (Scalar(this->samples));
        return buffer;
    }

  private:
    /** @brief render with rolling shutter and motion blur */
    inline void render_sample(const unsigned long sample, RawImage& buffer,
                              const Geometry<DIMENSION>& geometry,
                              Scene<DIMENSION>& scene) const {

        for (unsigned long j = 0; j < this->canvas.height; ++j) {

            // update per line (rolling shutter + motion blur)
            const typename Scene<DIMENSION>::FreezeGuard& frozen_scene =
                scene.freeze_for_time(mid_frame_time(j));

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be
//       optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
            for (unsigned long i = 0; i < this->canvas.width; ++i) {
                this->render_pixel_sample(i, j, geometry, frozen_scene, buffer);
            }
        }

        if (this->frequent_render_callback) {
            this->frequent_render_callback(
                typename Renderer<DIMENSION>::State{buffer, sample});
        }
        if (this->infrequent_render_callback &&
            (sample % this->infrequent_callback_frequency == 0)) {
            this->infrequent_render_callback(
                typename Renderer<DIMENSION>::State{buffer, sample});
        }
    }

    /**
     * @brief returns a randomized time for the current image line
     * @note Realizes rolling shutter and motion blur.
     */
    inline Scalar mid_frame_time(const unsigned horizonal_line) const {
        auto res = this->time;
        // time shift linear with line position
        res += this->frame_exposure_time *
               (Scalar(horizonal_line) / Scalar(this->canvas.height));
        // random shift for motion blur
        res += random_scalar(0.0, this->total_line_exposure_time);
        return res;
    }
};

/** @brief rolling shutter render for 3D manifolds */
using RollingShutterRenderer3D = RollingShutterRenderer<Dimension{3}>;

} // namespace cpp_raytracing

#endif
