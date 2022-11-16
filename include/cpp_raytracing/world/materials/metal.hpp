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
template <Dimension DIMENSION>
class Metal : public Material<DIMENSION> {

  public:
    /** @brief color of the metal surface */
    std::shared_ptr<Texture<DIMENSION>> color;
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

    /** @see Material::scatter */
    std::pair<Vec3, Color>
    scatter(const HitRecord<DIMENSION>& record,
            const Vec3& onb_ray_direction) const override {
        using namespace tensor;

        const Vec3 para =
            dot(record.onb_normal, onb_ray_direction) * record.onb_normal;
        const Vec3 ortho = onb_ray_direction - para;
        const Vec3 direction = reflect(ortho, para, roughness);

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture<DIMENSION>::value_for_missing_texture(
                        record.uv_coordinates, record.point);

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

/** @brief metal material for entities in a 3D mnifold */
using Metal3D = Metal<Dimension{3}>;

} // namespace cpp_raytracing

#endif
