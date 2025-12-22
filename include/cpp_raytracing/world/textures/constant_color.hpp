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
template <Dimension DIMENSION>
class ColorTexture : public Texture<DIMENSION> {
  public:
    /** @brief color of the surface */
    Color color = Colors::WHITE;
    /** @brief default construct with default idenfifier root */
    ColorTexture() = default;

    /** @brief copy constructor */
    ColorTexture(const ColorTexture&) = delete;

    /** @brief move constructor */
    ColorTexture(ColorTexture&&) = default;

    /** @brief copy assignment */
    ColorTexture& operator=(const ColorTexture&) = delete;

    /** @brief move assignment */
    ColorTexture& operator=(ColorTexture&&) = default;

    ~ColorTexture() override = default;

    /** @see Texture::value */
    Color value([[maybe_unused]] const Vec2& coordinates,
                [[maybe_unused]] const Vec<DIMENSION>& point) const override {
        return color;
    }
};

/** @brief texture of constant color for 3D entity */
using ImageColor3D = ColorTexture<Dimension{3}>;

} // namespace cpp_raytracing

#endif
