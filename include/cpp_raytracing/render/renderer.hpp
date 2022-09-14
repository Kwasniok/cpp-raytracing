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
#include "../world/entities/entity.hpp"
#include "../world/materials/base.hpp"
#include "../world/ray.hpp"
#include "../world/scene.hpp"
#include "canvas.hpp"
#include "image.hpp"

namespace cpp_raytracing {

/**
 * @brief image renderer
 */
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
     * @note Each interaction with an object creates a new segment even if not
     *      apparent in the final image.
     *      There is no distinction between different types of ray segments.
     */
    unsigned long ray_depth = 1;

    /**
     * @brief color indicator for ray ended before hitting anything
     * @note May be used for global illumination or debugging.
     */
    Color ray_color_if_ray_ended{0.0, 1.0, 0.0};
    /** @brief color indicator for missing material */
    Color ray_color_if_no_material{1.0, 0.0, 1.0};

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
    unsigned long infrequent_callback_frequency = 10;

    /** @brief time the exposure of the frame starts */
    Scalar time = 0.0;

    /**
     * @brief minimal ray length
     * @note Must be strictly larger than zero.
     * @see maximal_ray_length
     */
    Scalar minimal_ray_length = 1e-5;
    /**
     * @brief minimal ray length
     * @note Must be strictly larger than minimal_ray_length.
     * @see minimal_ray_length
     */
    Scalar maximal_ray_length = infinity;

    /** @brief render Scene as RawImage */
    virtual RawImage render(const Geometry& geometry, Scene& scene) = 0;

    /** @brief calculates color of light ray */
    Color ray_color(const Geometry& geometry,
                    const Scene::FreezeGuard& frozen_scene, Ray* ray,
                    const unsigned long depth) const {

        // check depth limit
        if (depth == 0) {
            return ray_color_if_ray_ended;
        }

        // propagate ray
        const std::optional<RaySegment> current_opt_segment =
            ray->next_ray_segment();

        // extract next segment
        if (!current_opt_segment.has_value()) {
            // ray ended (e.g. boundary of space or technical limit)
            return ray_color_if_ray_ended;
        }
        // has next segment
        const RaySegment& current_segment = current_opt_segment.value();

        // collect hit record
        HitRecord record = frozen_scene.hit_record(
            geometry, current_segment, minimal_ray_length, maximal_ray_length);

        // check for hit with entity within current segment
        if (!current_segment.contains(record.t)) {
            // no hit within current segment
            return ray_color(geometry, frozen_scene, ray, depth - 1);
        }
        // detected hit with entity within cuurent segment

        // check for material
        if (!record.material) {
            // no material
            return ray_color_if_no_material;
        }
        // has material

        // cast secondary ray or emitter
        // TODO: geometry-dependent scattering
        const auto [scattered_ray_segment, color] =
            record.material->scatter(record, current_segment);
        if (scattered_ray_segment.direction_exactly_zero()) {
            // is emitter
            return color;
        } else {
            // cast secondary ray
            // TODO: remove hack
            EuclideanRay scattered_ray{scattered_ray_segment.start(),
                                       scattered_ray_segment.direction()};
            return color *
                   ray_color(geometry, frozen_scene, &scattered_ray, depth - 1);
        }
    }

  protected:
    /** @brief render a single sample for a single pixel */
    inline void render_pixel_sample(const unsigned long i,
                                    const unsigned long j,
                                    const Geometry& geometry,
                                    const Scene::FreezeGuard& frozen_scene,
                                    RawImage& buffer) const {
        // random sub-pixel offset for antialiasing
        Scalar x = Scalar(i) + random_scalar(-0.5, +0.5);
        Scalar y = Scalar(j) + random_scalar(-0.5, +0.5);
        // transform to camera coordinates
        x = (2.0 * x / canvas.width - 1.0);
        y = (2.0 * y / canvas.height - 1.0);

        std::unique_ptr<Ray> ray =
            frozen_scene.active_camera.ray_for_coords(x, y);
        const Color pixel_color =
            ray_color(geometry, frozen_scene, ray.get(), ray_depth);
        buffer[{i, j}] += pixel_color;
    }
};

/** @brief renderer with global shutter and motion blur */
class GlobalShutterRenderer : public Renderer {
  public:
    /**
     * @brief total duration of the frame's exposure
     * @note `0.0` means no motion blur
     * @note should be smaller than the distance of two frames
     *        to be realistic
     */
    Scalar exposure_time = 0.0;

    virtual RawImage render(const Geometry& geometry, Scene& scene) override {

        RawImage buffer{canvas.width, canvas.height};

        // optimization: reduce cache generation if possible
        if (exposure_time == 0.0) {
            // note: ideal image: no motion blur -> no nned for repeated
            //       temporal updates
            const Scene::FreezeGuard& frozen_scene = scene.freeze_for_time(
                random_scalar(time, time + exposure_time));

            for (unsigned long s = 1; s < samples + 1; ++s) {
                render_sample(s, buffer, geometry, frozen_scene);
            }

        } else {
            // with motion blur
            for (unsigned long s = 1; s < samples + 1; ++s) {

                // scene for random time in exposure window
                const Scene::FreezeGuard& frozen_scene = scene.freeze_for_time(
                    random_scalar(time, time + exposure_time));

                render_sample(s, buffer, geometry, frozen_scene);
            }
        }

        buffer *= 1 / (Scalar(samples));
        return buffer;
    }

  private:
    /** @brief render with global shutter and motion blur */
    inline void render_sample(const unsigned long sample, RawImage& buffer,
                              const Geometry& geometry,
                              const Scene::FreezeGuard& frozen_scene) {

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
        for (unsigned long j = 0; j < canvas.height; ++j) {
            for (unsigned long i = 0; i < canvas.width; ++i) {
                render_pixel_sample(i, j, geometry, frozen_scene, buffer);
            }
        }

        if (frequent_render_callback) {
            frequent_render_callback(State{buffer, sample});
        }
        if (infrequent_render_callback &&
            (sample % infrequent_callback_frequency == 0)) {
            infrequent_render_callback(State{buffer, sample});
        }
    }
};

/** @brief renderer with rolling shutter and motion blur */
class RollingShutterRenderer : public Renderer {
  public:
    /**
     * @brief total duration of the frame's exposure
     * @note `0.0` means instant image
     * @note should be larger than zero to be realistic
     * @note should be smaller than the distance of two frames
     *       to be realistic
     */
    Scalar exposure_time = 0.0;

    /**
     * @brief exposure time per line scanned times total number of lines
     * @note `0.0` means no motion blur
     * @note should be (much) smaller than the distance of two frames
     *       to be realistic
     */
    Scalar motion_blur = 0.0;

    virtual RawImage render(const Geometry& geometry, Scene& scene) override {

        RawImage buffer{canvas.width, canvas.height};

        for (unsigned long s = 1; s < samples + 1; ++s) {
            render_sample(s, buffer, geometry, scene);
        }

        buffer *= 1 / (Scalar(samples));
        return buffer;
    }

  private:
    /** @brief render with rolling shutter and motion blur */
    inline void render_sample(const unsigned long sample, RawImage& buffer,
                              const Geometry& geometry, Scene& scene) const {

        for (unsigned long j = 0; j < canvas.height; ++j) {

            // update scene per line (rolling shutter + motion blur)
            const Scene::FreezeGuard& frozen_scene =
                scene.freeze_for_time(mid_frame_time(j));

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be
//       optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
            for (unsigned long i = 0; i < canvas.width; ++i) {
                render_pixel_sample(i, j, geometry, frozen_scene, buffer);
            }
        }

        if (frequent_render_callback) {
            frequent_render_callback(State{buffer, sample});
        }
        if (infrequent_render_callback &&
            (sample % infrequent_callback_frequency == 0)) {
            infrequent_render_callback(State{buffer, sample});
        }
    }

    /**
     * @brief returns a randomized time for the current image line
     * @note Realizes rolling shutter and motion blur.
     */
    inline Scalar mid_frame_time(const unsigned horizonal_line) const {
        auto res = time;
        // time shift linear with line position
        res += exposure_time * (Scalar(horizonal_line) / Scalar(canvas.height));
        // random shift for motion blur
        res += random_scalar(0.0, motion_blur);
        return res;
    }
};

} // namespace cpp_raytracing

#endif
