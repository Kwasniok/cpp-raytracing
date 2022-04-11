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
    const auto scale = 1.0 / samples;
    const auto r = int_from_color_scalar(color.r() * scale);
    const auto g = int_from_color_scalar(color.g() * scale);
    const auto b = int_from_color_scalar(color.b() * scale);
    os << r << " " << g << " " << b;
    return os;
}

Color constexpr ray_back_ground_color(const Ray& ray) {
    Vec3 direction = ray.direction();
    direction = unit_vector(direction);
    auto t = 0.5 * std::abs(direction.y()) + 0.25;
    Color color = (1.0 - t) * Colors::WHITE + t * Colors::BLUE;
    return color;
}

Color normal_color(const HitRecord& record) {
    return 0.5 * Color(record.normal.x() + 1, record.normal.y() + 1,
                       record.normal.z() + 1);
}

Color ray_color(const Scene& hittables, const Ray& ray) {
    HitRecord closest_record = {.t = SCALAR_INF};

    for (const auto& hittable : hittables) {
        HitRecord record = hittable->hit_record(ray);
        if (record.t < closest_record.t) {
            closest_record = record;
        }
    }

    if (closest_record.t < SCALAR_INF) {
        return normal_color(closest_record);
    }
    return ray_back_ground_color(ray);
}

} // namespace ray

#endif
