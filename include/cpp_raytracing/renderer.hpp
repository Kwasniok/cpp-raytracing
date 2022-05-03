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
#include "hittables.hpp"
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
    /** @brief callback type used by Renderer::render() */
    using RenderCallbackFunc = void (*)(const RawImage& current_image,
                                        const unsigned long current_samples);

    /** @brief color indicator for missing material */
    constexpr static Color RAY_COLOR_NO_MATERIAL{1.0, 0.0, 1.0};

    /** @brief render Scene as RawImage */
    RawImage render(const Canvas& canvas, const Scene& scene) {
        const Camera& camera = scene.camera;

        RawImage image{canvas.width, canvas.height};

        for (unsigned long s = 0; s < samples; ++s) {
            // note: Mind the memory layout of image and data acces!
            //       Static schedule with small chunksize seems to be optimal.
#pragma omp parallel for shared(scene, camera, image) schedule(static, 1)
            for (unsigned long j = 0; j < canvas.height; ++j) {
                for (unsigned long i = 0; i < canvas.width; ++i) {
                    // random sub-pixel offset for antialiasing
                    Scalar x = Scalar(i) + random_scalar(-0.5, +0.5);
                    Scalar y = Scalar(j) + random_scalar(-0.5, +0.5);
                    // transform to camera coordinates
                    x = (2.0 * x / canvas.width - 1.0);
                    y = (2.0 * y / canvas.height - 1.0);

                    const Ray ray = camera.ray_for_coords(x, y);
                    const Color pixel_color = ray_color(scene, ray, ray_depth);
                    image[{i, j}] += pixel_color;
                }
            }
            if (render_callback) {
                render_callback(image, s + 1);
            }
        }

        image *= 1 / (Scalar(samples));
        return image;
    }

    /** @brief calculates color of light ray */
    static Color ray_color(const Scene& scene, const Ray& ray,
                           const unsigned long depth) {
        if (depth == 0) {
            return Colors::BLACK;
        }
        HitRecord record = scene.hit_record(ray, 0.00001);
        if (!(record.t < infinity)) {
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
     *        to display the current sate of the image
     */
    RenderCallbackFunc render_callback;
};

} // namespace cpp_raytracing

#endif
