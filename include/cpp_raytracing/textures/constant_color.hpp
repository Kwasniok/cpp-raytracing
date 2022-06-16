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

    ConstantColor() = default;
    ConstantColor(const Color& color) : color(color){};
    ConstantColor(Color&& color) : color(std::move(color)){};

    virtual ~ConstantColor() = default;

    virtual Color value(const Vec2& coordinates,
                        const Vec3& point) const override {
        return color;
    }
};

} // namespace cpp_raytracing

#endif
