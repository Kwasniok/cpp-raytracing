/**
 * @file
 * @brief 3D checker texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CHECKER_3D_HPP
#define CPP_RAYTRACING_TEXTURES_CHECKER_3D_HPP

#include <cmath>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple 3D checker texture
 */
class Checker3D : public Texture {
  public:
    /** @brief primary color of the surface */
    Color color1 = Colors::WHITE;
    /** @brief secondary color of the surface */
    Color color2 = Colors::BLACK;
    /** @brief offset */
    Vec3 offset = Vec3{0.0, 0.0, 0.0};
    /** @brief size of each box in uv coordinates */
    Scalar scale = 1.0;
    /** @brief default construct with default idenfifier root */
    Checker3D() = default;

    /** @brief copy constructor */
    Checker3D(const Checker3D&) = delete;

    /** @brief move constructor */
    Checker3D(Checker3D&&) = default;

    /** @brief copy assignment */
    Checker3D& operator=(const Checker3D&) = delete;

    /** @brief move assignment */
    Checker3D& operator=(Checker3D&&) = default;

    ~Checker3D() override = default;

    Color value([[maybe_unused]] const Vec2& coordinates,
                const Vec3& point) const override {

        const Vec3 p = (point - offset) * (pi / scale);
        const auto val = std::sin(p.x()) * std::sin(p.y()) * std::sin(p.z());

        const bool is_primary = val < 0.0;

        return is_primary ? color1 : color2;
    }
};

} // namespace cpp_raytracing

#endif
