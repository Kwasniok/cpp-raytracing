/**
 * @file
 * @brief 2D checker texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP
#define CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP

#include <cmath>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple 2D checker texture
 */
class Checker2D : public Texture {
  public:
    /** @brief primary color of the surface */
    Color color1 = Colors::WHITE;
    /** @brief secondary color of the surface */
    Color color2 = Colors::BLACK;
    /** @brief offset */
    Vec2 offset = Vec2{0.0, 0.0};
    /** @brief size of each box in uv coordinates */
    Scalar scale = 1.0;

    /** @brief default construct with default idenfifier root */
    Checker2D() = default;

    /** @brief copy constructor */
    Checker2D(const Checker2D&) = delete;

    /** @brief move constructor */
    Checker2D(Checker2D&&) = default;

    /** @brief copy assignment */
    Checker2D& operator=(const Checker2D&) = delete;

    /** @brief move assignment */
    Checker2D& operator=(Checker2D&&) = default;

    ~Checker2D() override = default;

    Color value(const Vec2& coordinates,
                [[maybe_unused]] const Vec3& point) const override {

        const Vec2 c = (coordinates - offset) * (pi / scale);
        const auto val = std::sin(c.u()) * std::sin(c.v());

        const bool is_primary = val < 0.0;

        return is_primary ? color1 : color2;
    }
};

} // namespace cpp_raytracing

#endif
