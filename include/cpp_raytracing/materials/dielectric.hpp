/**
 * @file
 * @brief dielectric material
 */

#ifndef CPP_RAYTRACING_MATERIALS_DIELECTRIC_HPP
#define CPP_RAYTRACING_MATERIALS_DIELECTRIC_HPP

#include <cmath>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief colored translucent dielectric material
 */
class Dielectric : public Material {
  public:
    /** @brief initialize with identifier and parameters */
    Dielectric(Identifier<Material>&& id, const Color& color,
               const Scalar index_of_refraction)
        : Material(std::move(id)),
          color(color),
          index_of_refraction(index_of_refraction) {}
    /**
     * @brief initialize with (similar) identifier and parameters
     * @see Identifier::make_always
     */
    Dielectric(const char* id, const Color& color,
               const Scalar index_of_refraction)
        : Material(std::move(id)),
          color(color),
          index_of_refraction(index_of_refraction) {}
    /**@brief initialize with parameters */
    Dielectric(const Color& color, const Scalar index_of_refraction)
        : Material(), color(color), index_of_refraction(index_of_refraction) {}

    /** @brief move constructor */
    Dielectric(Dielectric&&) = default;
    /** @brief move operation */
    Dielectric& operator=(Dielectric&&) = default;
    Dielectric(const Dielectric&) = delete;
    Dielectric& operator=(const Dielectric&) = delete;

    virtual ~Dielectric() = default;

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        // note: This algorithm assumes vacuum to medium transitions and
        // vice versa
        //       only.
        const Scalar refraction_ratio = record.front_face
                                            ? (1.0 / index_of_refraction)
                                            : index_of_refraction;
        const Vec3 unit_direction = unit_vector(ray.direction());
        const auto cos_theta = -dot(record.normal, unit_direction);
        const auto sin_theta_squared = std::abs(1.0 - std::pow(cos_theta, 2));
        Vec3 para = -cos_theta * record.normal;
        Vec3 ortho = unit_direction - para;

        Vec3 direction;
        //  Snell's law
        const bool cannot_refract =
            std::pow(refraction_ratio, 2) * sin_theta_squared > 1.0;
        // Scklick approximation
        const bool ray_reflects =
            cannot_refract ||
            reflectance(cos_theta, refraction_ratio) > random_scalar(0.0, 1.0);
        if (ray_reflects) {
            direction = reflect(ortho, para);
        } else {
            direction = refract(ortho, record.normal, refraction_ratio);
        }
        return {Ray(record.point, direction), color};
    }

  private:
    /** @brief reflected ray */
    static constexpr Vec3 reflect(const Vec3 ortho, const Vec3 para) {
        return ortho - para;
    }
    /** @brief refracted ray based on Snell's law */
    static constexpr Vec3 refract(Vec3 ortho, const Vec3 normal,
                                  const Scalar index_of_refraction) {

        ortho *= index_of_refraction;
        const Vec3 para =
            -std::sqrt(std::abs(1.0 - ortho.length_squared())) * normal;
        return ortho + para;
    }

    /** @brief reflectance value based on Schlick's approximation */
    static Scalar reflectance(const Scalar cos_theta,
                              const Scalar index_of_refraction) {
        auto x = (1.0 - index_of_refraction) / (1.0 + index_of_refraction);
        x *= x;
        return x + (1.0 - x) * std::pow(1 - cos_theta, 5);
    }

  public:
    /** @brief color of the dielectric */
    Color color = Colors::WHITE;
    /**
     * @brief index of refraction
     * @note 1.0=air, >1.0=typical, <1.0=atypical
     */
    Scalar index_of_refraction = 1.0;
};
} // namespace cpp_raytracing

#endif
