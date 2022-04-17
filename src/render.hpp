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

constexpr Color ray_back_ground_color(const Ray& ray) {
    Vec3 direction = ray.direction();
    direction = unit_vector(direction);
    auto t = 0.5 * (std::abs(direction.y()) + 1.0);
    Color color = (1.0 - t) * Colors::WHITE + t * Color(0.5, 0.7, 1.0);
    return color;
}

constexpr Color RAY_COLOR_NO_MATERIAL{1.0, 0.0, 1.0};

Color ray_color(const Scene& scene, const Ray& ray, const unsigned long depth) {
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
    const auto [scattered_ray, color] = record.material->scatter(record, ray);
    if (scattered_ray.direction_near_zero(1.0e-12)) {
        return color;
    } else {
        return color * ray_color(scene, scattered_ray, depth - 1);
    }
}

} // namespace ray

#endif
