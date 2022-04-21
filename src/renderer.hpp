/**
 * @file
 * @brief renderer
 */

#ifndef CPP_RAYTRACING_RENDER_H
#define CPP_RAYTRACING_RENDER_H

#include <cmath>
#include <iostream>
#include <omp.h>

#include "color.hpp"
#include "hittable.hpp"
#include "image.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "util.hpp"

namespace ray {

/**
 * @brief image renderer
 */
class Renderer {

  public:
    using RenderCallbackFunc = void (*)(const RawImage& current_image,
                                        const unsigned long current_samples);

    /** @brief color indicator for missing material */
    constexpr static Color RAY_COLOR_NO_MATERIAL{1.0, 0.0, 1.0};

    /** @brief render Scene as RawImage */
    RawImage render(const Scene& scene) {
        const Camera& camera = scene.camera;

        // render samples in chunks and distribute the workload among multiple
        // threads

        // final result buffer
        RawImage image{camera.canvas_width, camera.canvas_height};
        // buffer for each chunk
        // note: after each chunk the results are collected here
        RawImage chunk_image{camera.canvas_width, camera.canvas_height};

        const unsigned long samples_chunk_size = 16; // TODO: make configurable
        unsigned long current_samples = 0;

        for (unsigned long samples_chunk = 0;
             (samples - current_samples) != 0; // remaining samples
             ++samples_chunk) {

            const unsigned long samples_in_current_chunk =
                std::min(samples_chunk_size, (samples - current_samples));

#pragma omp parallel for reduction(+ : chunk_image)
            for (unsigned long s = 0; s < samples_in_current_chunk; ++s) {
                for (unsigned long j = 0; j < camera.canvas_height; ++j) {
                    for (unsigned long i = 0; i < camera.canvas_width; ++i) {
                        // random sub-pixel offset for antialiasing
                        Scalar x = Scalar(i) + random_scalar<-0.5, +0.5>();
                        Scalar y = Scalar(j) + random_scalar<-0.5, +0.5>();
                        // transform to camera coordinates
                        x = (2.0 * x / camera.canvas_width - 1.0);
                        y = (2.0 * y / camera.canvas_height - 1.0);

                        Ray ray = camera.ray_for_coords(x, y);
                        Color pixel_color = ray_color(scene, ray, ray_depth);
                        image[{i, j}] += pixel_color;
                    }
                }
                }
            }
            // update final image buffer with chunk image
            image += chunk_image;
            current_samples += samples_in_current_chunk;
            if (render_callback) {
                render_callback(image, current_samples);
            }
        }

        const Scalar scale = 1 / (Scalar(samples));
        image *= scale;

        return image;
    }

    /** @brief calculates color of light ray */
    static Color ray_color(const Scene& scene, const Ray& ray,
                           const unsigned long depth) {
        if (depth == 0) {
            return Colors::BLACK;
        }
        HitRecord record = scene.hit_record(ray, 0.00001);
        if (!(record.t < SCALAR_INF)) {
            return ray_back_ground_color(ray);
        }
        if (!record.material) {
            return RAY_COLOR_NO_MATERIAL;
        }
        const auto [scattered_ray, color] =
            record.material->scatter(record, ray);
        if (scattered_ray.direction_near_zero(1.0e-12)) {
            return color;
        } else {
            return color * ray_color(scene, scattered_ray, depth - 1);
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

  public:
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
     * @brief callback function to be called regularly during to rendering e.g.
     * to display the current sate of the image
     */
    RenderCallbackFunc render_callback;
};

} // namespace ray

#endif
