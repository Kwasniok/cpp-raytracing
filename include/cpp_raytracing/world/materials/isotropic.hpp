/**
 * @file
 * @brief isotropic material
 */

#ifndef CPP_RAYTRACING_MATERIALS_ISOTROPIC_HPP
#define CPP_RAYTRACING_MATERIALS_ISOTROPIC_HPP

#include "../textures/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple isotropic material (e.g. for volumes)
 * @note Suitable for volume materials.
 */
class Isotropic : public Material {
  public:
    /** @brief color of the isotropic 'surface' */
    std::shared_ptr<Texture3D> color;

    /** @brief default construct with default idenfifier root */
    Isotropic() = default;

    /** @brief copy constructor */
    Isotropic(const Isotropic&) = delete;

    /** @brief move constructor */
    Isotropic(Isotropic&&) = default;

    /** @brief copy assignment */
    Isotropic& operator=(const Isotropic&) = delete;

    /** @brief move assignment */
    Isotropic& operator=(Isotropic&&) = default;

    ~Isotropic() override = default;

    std::pair<Vec3, Color>
    scatter(const HitRecord3D& record,
            [[maybe_unused]] const Vec3& ray_direction) const override {

        using namespace tensor;

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture3D::value_for_missing_texture(record.uv_coordinates,
                                                         record.point);

        return {random_vec_inside_unit_sphere<3_D>(), color_value};
    }
};

} // namespace cpp_raytracing

#endif
