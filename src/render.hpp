#ifndef CPP_RAYTRACING_RENDER_H
#define CPP_RAYTRACING_RENDER_H value

#include <cmath>

#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "scene.hpp"

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

Color ray_color(const Scene& scene, const Ray& ray, unsigned long depth) {
    if (depth == 0) {
        return Colors::BLACK;
    }
    HitRecord record = scene.hit_record(ray);
    const Color surface_color{0.5, 0.5, 0.5};
    if (record.t < SCALAR_INF) {
        // diffuse surface
        Vec3 direction = record.normal + random_vector_in_unit_sphere();
        Color color = ray_color(scene, Ray(record.point, direction), depth - 1);
        color *= surface_color;
        return color;
    }
    return ray_back_ground_color(ray);
}

} // namespace ray

#endif
