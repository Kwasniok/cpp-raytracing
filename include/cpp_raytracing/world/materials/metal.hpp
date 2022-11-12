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

    /** @brief default construct with default idenfifier root */
    Metal() = default;

    /** @brief copy constructor */
    Metal(const Metal&) = delete;

    /** @brief move constructor */
    Metal(Metal&&) = default;

    /** @brief copy assignment */
    Metal& operator=(const Metal&) = delete;

    /** @brief move assignment */
    Metal& operator=(Metal&&) = default;

    ~Metal() override = default;

    std::pair<Vec3, Color> scatter(const HitRecord3D& record,
                                   const Vec3& ray_direction) const override {
        using namespace tensor;

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
        using namespace tensor;

        return ortho - para + roughness * random_vec_inside_unit_sphere<3_D>();
    }
};

} // namespace cpp_raytracing

#endif
