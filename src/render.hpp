#ifndef CPP_RAYTRACING_RENDER_H
#define CPP_RAYTRACING_RENDER_H value

#include <cmath>

#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "util.hpp"

namespace ray {

inline std::ostream& write_color_as_int_triple(std::ostream& os,
                                               const Color& color,
                                               const unsigned long samples) {
    // rescale
    const auto scale = 1.0 / samples;
    auto r = color.r() * scale;
    auto g = color.g() * scale;
    auto b = color.b() * scale;
    // gamma correction (raise to the power of 1/gamma)
    r = std::sqrt(r);
    g = std::sqrt(g);
    b = std::sqrt(b);
    // convert to integers
    const auto ir = int_from_color_scalar(r);
    const auto ig = int_from_color_scalar(g);
    const auto ib = int_from_color_scalar(b);
    os << ir << " " << ig << " " << ib;
    return os;
}

Color constexpr ray_back_ground_color(const Ray& ray) {
    Vec3 direction = ray.direction();
    direction = unit_vector(direction);
    auto t = 0.5 * std::abs(direction.y()) + 1.0;
    Color color = (1.0 - t) * Colors::WHITE + t * Color(0.5, 0.7, 1.0);
    return color;
}

Color normal_color(const HitRecord& record) {
    return 0.5 * Color(record.normal.x() + 1, record.normal.y() + 1,
                       record.normal.z() + 1);
}

Color ray_color(const Scene& scene, const Ray& ray, unsigned long depth,
                unsigned long depth_reflection) {
    if (depth == 0) {
        return Colors::BLACK;
    }
    HitRecord record = scene.hit_record(ray, 0.00001);
    constexpr Color COLOR_NO_MATERIAL{1.0, 0.0, 1.0};
    if (record.t < SCALAR_INF) {
        if (!record.material) {
            return COLOR_NO_MATERIAL;
        }
        const Material& material = *record.material;
        Color color;
        // diffuse
        if ((1.0 - material.reflection) != 0.0) {
            Vec3 direction = record.normal + random_unit_vector();
            if (direction.near_zero(1.0e-12)) {
                // in case of normal and random vector beeing antiparallel
                // use normal instead
                direction = record.normal;
            }
            const Color diff_ray_color =
                ray_color(scene, Ray(record.point, direction), depth - 1,
                          depth_reflection);
            color += (1 - material.reflection) *
                     (material.diffuse_color * diff_ray_color);
        }
        // reflection
        if (material.reflection != 0.0) {
            const Vec3 ray_para =
                dot(record.normal, ray.direction()) * record.normal;
            const Vec3 ray_ortho = ray.direction() - ray_para;
            const Vec3 direction = ray_ortho - ray_para;
            const Color ref_ray_col =
                ray_color(scene, Ray(record.point, direction),
                          clip(depth, 0ul, depth_reflection), 0);
            color +=
                material.reflection * (material.reflection_color * ref_ray_col);
        }
        return color;
    }
    return ray_back_ground_color(ray);
}

} // namespace ray

#endif
