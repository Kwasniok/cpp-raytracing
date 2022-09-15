/**
 * @file
 * @brief metal material
 */

#ifndef CPP_RAYTRACING_MATERIALS_METAL_HPP
#define CPP_RAYTRACING_MATERIALS_METAL_HPP

#include "../textures/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple colored metal material
 */
class Metal : public Material {

  public:
    /** @brief color of the metal surface */
    std::shared_ptr<Texture> color;
    /**
     * @brief roughness of the diffuse surface
     * note: `roughness=0.0...1.0`
     */
    Scalar roughness = 0.0;

    virtual ~Metal() = default;

    virtual std::pair<Vec3, Color>
    scatter(const HitRecord& record, const Vec3& ray_direction) const override {
        const Vec3 para = dot(record.normal, ray_direction) * record.normal;
        const Vec3 ortho = ray_direction - para;
        const Vec3 direction = reflect(ortho, para, roughness);

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture::value_for_missing_texture(record.uv_coordinates,
                                                       record.point);

        return {direction, color_value};
    }

  private:
    /** @brief reflected ray */
    static inline Vec3 reflect(const Vec3 ortho, const Vec3 para,
                               const Scalar roughness) {
        return ortho - para + roughness * random_vector_in_unit_sphere();
    }
};

} // namespace cpp_raytracing

#endif
