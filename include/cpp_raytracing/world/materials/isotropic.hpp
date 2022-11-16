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
template <Dimension DIMENSION>
class Isotropic : public Material<DIMENSION> {
  public:
    /** @brief color of the isotropic 'surface' */
    std::shared_ptr<Texture<DIMENSION>> color;

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

    /** @see Material::scatter */
    std::pair<Vec3, Color>
    scatter(const HitRecord<DIMENSION>& record,
            [[maybe_unused]] const Vec3& onb_ray_direction) const override {

        using namespace tensor;

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture<DIMENSION>::value_for_missing_texture(
                        record.uv_coordinates, record.point);

        return {random_vec_inside_unit_sphere<3_D>(), color_value};
    }
};

/** @brief iostropic material for entities in a 3D mnifold */
using Isotropic3D = Isotropic<Dimension{3}>;

} // namespace cpp_raytracing

#endif
