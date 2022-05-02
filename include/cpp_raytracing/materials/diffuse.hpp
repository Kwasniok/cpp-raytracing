/**
 * @file
 * @brief diffuse material
 */

#ifndef CPP_RAYTRACING_MATERIALS_DIFFUSE_HPP
#define CPP_RAYTRACING_MATERIALS_DIFFUSE_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple Lambertian colored diffuse material
 */
class Diffuse : public Material {
  public:
    /** @brief Scalars below this threshold are considered to be zero.*/
    constexpr static Scalar epsilon = 1.0e-12;

    /** @brief initialize with parameters */
    Diffuse(const Color& color) : color(color) {}
    Diffuse(const Diffuse&) = default;
    Diffuse(Diffuse&&) = default;
    Diffuse& operator=(const Diffuse&) = default;
    Diffuse& operator=(Diffuse&&) = default;
    virtual ~Diffuse() = default;

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        Vec3 direction = record.normal + random_unit_vector();
        if (direction.near_zero(epsilon)) {
            // in case of normal and random vector beeing antiparallel
            // use normal instead
            direction = record.normal;
        }
        return {Ray(record.point, direction), color};
    }

  public:
    /** @brief color of the diffuse surface */
    Color color = Colors::WHITE;
};

} // namespace cpp_raytracing

#endif
