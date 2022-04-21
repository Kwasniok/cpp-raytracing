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
        RawImage image{scene.camera.canvas_width, scene.camera.canvas_height};

        // break up image into chunks and render them individually per sample
        const int num_procs = omp_get_num_procs();
        const unsigned long chunk_size_x =
            scene.camera.canvas_width / num_procs;
        const unsigned long chunk_size_y =
            scene.camera.canvas_height / num_procs;
        const unsigned long x_chunks =
            ceil_idiv(scene.camera.canvas_width, chunk_size_x);
        const unsigned long y_chunks =
            ceil_idiv(scene.camera.canvas_height, chunk_size_y);

        for (unsigned long s = 0; s < samples; ++s) {
#pragma omp for collapse(2)
            for (unsigned long chunk_y = 0; chunk_y < y_chunks; ++chunk_y) {
                for (unsigned long chunk_x = 0; chunk_x < x_chunks; ++chunk_x) {
                    const unsigned long x_start = chunk_x * chunk_size_x;
                    const unsigned long y_start = chunk_y * chunk_size_y;
                    const unsigned long x_end =
                        std::min((chunk_x + 1) * chunk_size_x,
                                 scene.camera.canvas_width);
                    const unsigned long y_end =
                        std::min((chunk_y + 1) * chunk_size_y,
                                 scene.camera.canvas_height);
                    update_chunk_sample(scene, image, x_start, x_end, y_start,
                                        y_end);
                }
            }

            if (render_callback) {
                render_callback(image, s + 1);
            }
        }

        image *= 1 / (Scalar(samples));
        return image;
    }

  private:
    void update_chunk_sample(const Scene& scene, RawImage& image,
                             const unsigned long x_start,
                             const unsigned long x_end,
                             const unsigned long y_start,
                             const unsigned long y_end) {
        const Camera& camera = scene.camera;

        for (unsigned long j = y_start; j < y_end; ++j) {
            for (unsigned long i = x_start; i < x_end; ++i) {
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

  public:
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
