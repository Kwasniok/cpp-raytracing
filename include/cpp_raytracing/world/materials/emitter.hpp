/**
 * @file
 * @brief emitter material
 */

#ifndef CPP_RAYTRACING_MATERIALS_EMITTER_HPP
#define CPP_RAYTRACING_MATERIALS_EMITTER_HPP

#include "../textures/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple colored emitter material
 */
class Emitter : public Material {
  public:
    /** @brief color of the emitting surface */
    std::shared_ptr<Texture> color;

    virtual ~Emitter() = default;

    virtual std::pair<Vec3, Color>
    scatter(const HitRecord& record, const Vec3& ray_direction) const override {
        const Vec3 direction = Vec3::zero(); // emissive

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture::value_for_missing_texture(record.uv_coordinates,
                                                       record.point);

        return {direction, color_value};
    }
};

} // namespace cpp_raytracing

#endif
