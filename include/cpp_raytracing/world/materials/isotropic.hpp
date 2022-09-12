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
    std::shared_ptr<Texture> color;

    virtual ~Isotropic() = default;

    virtual std::pair<RaySegment, Color> scatter(const HitRecord& record,
                                          const RaySegment& ray) const override {

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture::value_for_missing_texture(record.uv_coordinates,
                                                       record.point);

        return {RaySegment(record.point, random_vector_in_unit_sphere()), color_value};
    }
};

} // namespace cpp_raytracing

#endif
