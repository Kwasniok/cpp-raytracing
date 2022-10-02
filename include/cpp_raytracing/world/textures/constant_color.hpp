/**
 * @file
 * @brief constant color texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CONSTANT_COLOR_HPP
#define CPP_RAYTRACING_TEXTURES_CONSTANT_COLOR_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple constant color texture
 */
class ConstantColor : public Texture {
  public:
    /** @brief color of the surface */
    Color color = Colors::WHITE;
    /** @brief default construct with default idenfifier root */
    ConstantColor() = default;

    /** @brief copy constructor */
    ConstantColor(const ConstantColor&) = delete;

    /** @brief move constructor */
    ConstantColor(ConstantColor&&) = default;

    /** @brief copy assignment */
    ConstantColor& operator=(const ConstantColor&) = delete;

    /** @brief move assignment */
    ConstantColor& operator=(ConstantColor&&) = default;

    ~ConstantColor() override = default;

    Color value([[maybe_unused]] const Vec2& coordinates,
                [[maybe_unused]] const Vec3& point) const override {
        return color;
    }
};

} // namespace cpp_raytracing

#endif
