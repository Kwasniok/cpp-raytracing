/**
 * @file
 * @brief renderer
 */

#ifndef CPP_RAYTRACING_RENDER_HPP
#define CPP_RAYTRACING_RENDER_HPP

#include <cmath>
#include <omp.h>

#include "canvas.hpp"
#include "color.hpp"
#include "entities/entity.hpp"
#include "image.hpp"
#include "materials/base.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "util.hpp"

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
    using RenderCallbackFunc = void (*)(const State& current_state);

    /** @brief color indicator for missing material */
    constexpr static Color RAY_COLOR_NO_MATERIAL{1.0, 0.0, 1.0};

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

    /** @brief render Scene as RawImage */
    virtual RawImage render(Scene& scene) = 0;

    /** @brief calculates color of light ray */
    static Color ray_color(const Scene::FreezeGuard& frozen_scene,
                           const Ray& ray, const unsigned long depth) {
        if (depth == 0) {
            return Colors::BLACK;
        }
        HitRecord record = frozen_scene.hit_record(ray, 0.00001);
        if (!(record.t < infinity)) {
            return ray_back_ground_color(ray);
        }
        if (!record.material) {
            return RAY_COLOR_NO_MATERIAL;
        }
        const auto [scattered_ray, color] =
            record.material->scatter(record, ray);
        if (scattered_ray.direction_exactly_zero()) {
            // emissive material
            return color;
        } else {
            return color * ray_color(frozen_scene, scattered_ray, depth - 1);
        }
    }

    /** @brief calculates color of light ray hitting the background */
    constexpr static Color ray_back_ground_color(const Ray& ray) {
        Vec3 direction = ray.direction();
        direction = unit_vector(direction);
        auto t = 0.5 * (std::abs(direction.y()) + 1.0);
        Color color = (1.0 - t) * Colors::WHITE + t * Color(0.5, 0.7, 1.0);
        return color;
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

    virtual RawImage render(Scene& scene) override {

        RawImage buffer{canvas.width, canvas.height};

        // optimization: reduce cache generation if possible
        if (exposure_time == 0.0) {
            // note: ideal image: no motion blur -> no nned for repeated
            //       temporal updates
            const Scene::FreezeGuard& frozen_scene = scene.freeze_for_time(
                random_scalar(time, time + exposure_time));

            for (unsigned long s = 1; s < samples + 1; ++s) {
                render_sample(s, buffer, frozen_scene);
            }

        } else {
            // with motion blur
            for (unsigned long s = 1; s < samples + 1; ++s) {

                // scene for random time in exposure window
                const Scene::FreezeGuard& frozen_scene = scene.freeze_for_time(
                    random_scalar(time, time + exposure_time));

                render_sample(s, buffer, frozen_scene);
            }
        }

        buffer *= 1 / (Scalar(samples));
        return buffer;
    }

  private:
    /** @brief render with global shutter and motion blur */
    inline void render_sample(const unsigned long sample, RawImage& buffer,
                              const Scene::FreezeGuard& frozen_scene) {

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
        for (unsigned long j = 0; j < canvas.height; ++j) {
            for (unsigned long i = 0; i < canvas.width; ++i) {
                render_pixel_sample(i, j, frozen_scene, buffer);
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

    /** @brief render a single sample for a single pixel */
    inline void render_pixel_sample(const unsigned long i,
                                    const unsigned long j,
                                    const Scene::FreezeGuard& frozen_scene,
                                    RawImage& buffer) {
        // random sub-pixel offset for antialiasing
        Scalar x = Scalar(i) + random_scalar(-0.5, +0.5);
        Scalar y = Scalar(j) + random_scalar(-0.5, +0.5);
        // transform to camera coordinates
        x = (2.0 * x / canvas.width - 1.0);
        y = (2.0 * y / canvas.height - 1.0);

        const Ray ray = frozen_scene.active_camera.ray_for_coords(x, y);
        const Color pixel_color = ray_color(frozen_scene, ray, ray_depth);
        buffer[{i, j}] += pixel_color;
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

    virtual RawImage render(Scene& scene) override {

        RawImage buffer{canvas.width, canvas.height};

        for (unsigned long s = 1; s < samples + 1; ++s) {
            render_sample(s, buffer, scene);
        }

        buffer *= 1 / (Scalar(samples));
        return buffer;
    }

  private:
    /** @brief render with rolling shutter and motion blur */
    inline void render_sample(const unsigned long sample, RawImage& buffer,
                              Scene& scene) const {

        for (unsigned long j = 0; j < canvas.height; ++j) {

            // update scene per line (rolling shutter + motion blur)
            const Scene::FreezeGuard& frozen_scene =
                scene.freeze_for_time(mid_frame_time(j));

// note: Mind the memory layout of image buffer and data acces!
//       Static schedule with small chunksize seems to be
//       optimal.
#pragma omp parallel for shared(buffer) schedule(static, 1)
            for (unsigned long i = 0; i < canvas.width; ++i) {
                render_pixel_sample(i, j, frozen_scene, buffer);
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

    /** @brief render a single sample for a single pixel */
    inline void render_pixel_sample(const unsigned long i,
                                    const unsigned long j,
                                    const Scene::FreezeGuard& frozen_scene,
                                    RawImage& buffer) const {
        // random sub-pixel offset for antialiasing
        Scalar x = Scalar(i) + random_scalar(-0.5, +0.5);
        Scalar y = Scalar(j) + random_scalar(-0.5, +0.5);
        // transform to camera coordinates
        x = (2.0 * x / canvas.width - 1.0);
        y = (2.0 * y / canvas.height - 1.0);

        const Ray ray = frozen_scene.active_camera.ray_for_coords(x, y);
        const Color pixel_color = ray_color(frozen_scene, ray, ray_depth);
        buffer[{i, j}] += pixel_color;
    }
};

} // namespace cpp_raytracing

#endif
