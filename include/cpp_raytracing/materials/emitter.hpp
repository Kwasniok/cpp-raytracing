/**
 * @file
 * @brief emitter material
 */

#ifndef CPP_RAYTRACING_MATERIALS_EMITTER_HPP
#define CPP_RAYTRACING_MATERIALS_EMITTER_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple colored emitter material
 */
class Emitter : public Material {
  public:
    /** @brief initialize with parameters */
    Emitter(const Color& color) : color(color) {}
    virtual ~Emitter() = default;

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        const Vec3 direction{0.0, 0.0, 0.0};
        return {Ray(record.point, direction), color};
    }

  public:
    /** @brief color of the emitting surface */
    Color color = Colors::WHITE;
};

} // namespace cpp_raytracing

#endif