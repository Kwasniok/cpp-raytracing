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

    virtual ~Diffuse() = default;

    virtual std::pair<RaySegment, Color> scatter(const HitRecord& record,
                                          const RaySegment& ray) const override {
        Vec3 direction = record.normal + random_unit_vector();
        if (direction.near_zero(epsilon)) {
            // in case of normal and random vector beeing antiparallel
            // use normal instead
            direction = record.normal;
        }

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture::value_for_missing_texture(record.uv_coordinates,
                                                       record.point);

        return {RaySegment(record.point, direction), color_value};
    }
};

} // namespace cpp_raytracing

#endif
