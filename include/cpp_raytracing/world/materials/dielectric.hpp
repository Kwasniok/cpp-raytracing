/**
 * @file
 * @brief dielectric material
 */

#ifndef CPP_RAYTRACING_MATERIALS_DIELECTRIC_HPP
#define CPP_RAYTRACING_MATERIALS_DIELECTRIC_HPP

#include <cmath>

#include "../textures/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief colored translucent dielectric material
 */
template <Dimension DIMENSION>
class Dielectric : public Material<DIMENSION> {

  public:
    /** @brief color of the dielectric */
    std::shared_ptr<Texture<DIMENSION>> color;
    /**
     * @brief index of refraction
     * @note 1.0=air, >1.0=typical, <1.0=atypical
     */
    Scalar index_of_refraction = 1.0;

    /** @brief default construct with default idenfifier root */
    Dielectric() = default;

    /** @brief copy constructor */
    Dielectric(const Dielectric&) = delete;

    /** @brief move constructor */
    Dielectric(Dielectric&&) = default;

    /** @brief copy assignment */
    Dielectric& operator=(const Dielectric&) = delete;

    /** @brief move assignment */
    Dielectric& operator=(Dielectric&&) = default;

    ~Dielectric() override = default;

    /** @see Material::scatter */
    std::pair<Vec3, Color>
    scatter(const HitRecord<DIMENSION>& record,
            const Vec3& onb_ray_direction) const override {
        using namespace tensor;

        // note: This algorithm assumes vacuum to medium transitions and
        // vice versa
        //       only.
        const Scalar refraction_ratio = record.front_face
                                            ? (1.0 / index_of_refraction)
                                            : index_of_refraction;
        const Vec3 unit_direction = unit_vector(onb_ray_direction);
        const auto cos_theta = -dot(record.onb_normal, unit_direction);
        const auto sin_theta_squared = std::abs(1.0 - std::pow(cos_theta, 2));
        Vec3 para = -cos_theta * record.onb_normal;
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
            direction = refract(ortho, record.onb_normal, refraction_ratio);
        }

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture3D::value_for_missing_texture(record.uv_coordinates,
                                                         record.point);

        return {direction, color_value};
    }

  private:
    /** @brief reflected ray */
    static constexpr Vec3 reflect(const Vec3 ortho, const Vec3 para) {
        return ortho - para;
    }
    /** @brief refracted ray based on Snell's law */
    static constexpr Vec3 refract(Vec3 ortho, const Vec3 normal,
                                  const Scalar index_of_refraction) {

        using namespace tensor;

        ortho *= index_of_refraction;
        const Vec3 para =
            -std::sqrt(std::abs(1.0 - length_squared(ortho))) * normal;
        return ortho + para;
    }

    /** @brief reflectance value based on Schlick's approximation */
    static Scalar reflectance(const Scalar cos_theta,
                              const Scalar index_of_refraction) {
        auto x = (1.0 - index_of_refraction) / (1.0 + index_of_refraction);
        x *= x;
        return x + (1.0 - x) * std::pow(1 - cos_theta, 5);
    }
};

/** @brief dielectric material for entities in a 3D mnifold */
using Dielectric3D = Dielectric<Dimension{3}>;

} // namespace cpp_raytracing

#endif
