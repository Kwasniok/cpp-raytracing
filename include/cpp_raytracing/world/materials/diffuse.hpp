/**
 * @file
 * @brief diffuse material
 */

#ifndef CPP_RAYTRACING_MATERIALS_DIFFUSE_HPP
#define CPP_RAYTRACING_MATERIALS_DIFFUSE_HPP

#include "../textures/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple Lambertian colored diffuse material
 */
class Diffuse : public Material {
  public:
    /** @brief Scalars below this threshold are considered to be zero.*/
    constexpr static Scalar epsilon = 1.0e-12;

    /** @brief color of the diffuse surface */
    std::shared_ptr<Texture> color;

    /** @brief default construct with default idenfifier root */
    Diffuse() = default;

    /** @brief copy constructor */
    Diffuse(const Diffuse&) = delete;

    /** @brief move constructor */
    Diffuse(Diffuse&&) = default;

    /** @brief copy assignment */
    Diffuse& operator=(const Diffuse&) = delete;

    /** @brief move assignment */
    Diffuse& operator=(Diffuse&&) = default;

    ~Diffuse() override = default;

    std::pair<Vec3, Color>
    scatter(const HitRecord3D& record,
            [[maybe_unused]] const Vec3& ray_direction) const override {

        using namespace tensor;

        Vec3 direction = record.normal + random_unit_vec<3_D>();
        if (tensor::near_zero(direction, epsilon)) {
            // in case of normal and random vector beeing antiparallel
            // use normal instead
            direction = record.normal;
        }

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture::value_for_missing_texture(record.uv_coordinates,
                                                       record.point);

        return {direction, color_value};
    }
};

} // namespace cpp_raytracing

#endif
