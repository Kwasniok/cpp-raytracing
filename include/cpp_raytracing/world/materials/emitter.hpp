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
template <Dimension DIMENSION>
class Emitter : public Material<DIMENSION> {
  public:
    /** @brief color of the emitting surface */
    std::shared_ptr<Texture<DIMENSION>> color;

    /** @brief default construct with default idenfifier root */
    Emitter() = default;

    /** @brief copy constructor */
    Emitter(const Emitter&) = delete;

    /** @brief move constructor */
    Emitter(Emitter&&) = default;

    /** @brief copy assignment */
    Emitter& operator=(const Emitter&) = delete;

    /** @brief move assignment */
    Emitter& operator=(Emitter&&) = default;

    ~Emitter() override = default;

    /** @see Material::scatter */
    std::pair<Vec3, Color>
    scatter(const HitRecord<DIMENSION>& record) const override {
        using namespace tensor;

        const Vec3 direction = zero_vec<3_D>; // emissive

        const Color color_value =
            color ? color->value(record.uv_coordinates, record.point)
                  : Texture<DIMENSION>::value_for_missing_texture(
                        record.uv_coordinates, record.point);

        return {direction, color_value};
    }
};

/** @brief emitter material for entities in a 3D mnifold */
using Emitter3D = Emitter<Dimension{3}>;

} // namespace cpp_raytracing

#endif
