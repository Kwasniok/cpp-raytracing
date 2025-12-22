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
class ImageColor : public Texture<DIMENSION> {
  public:
    /** @brief color of the surface */
    Color color = Colors::WHITE;
    /** @brief default construct with default idenfifier root */
    ImageColor() = default;

    /** @brief copy constructor */
    ImageColor(const ImageColor&) = delete;

    /** @brief move constructor */
    ImageColor(ImageColor&&) = default;

    /** @brief copy assignment */
    ImageColor& operator=(const ImageColor&) = delete;

    /** @brief move assignment */
    ImageColor& operator=(ImageColor&&) = default;

    ~ImageColor() override = default;

    /** @see Texture::value */
    Color value([[maybe_unused]] const Vec2& coordinates,
                [[maybe_unused]] const Vec<DIMENSION>& point) const override {
        return color;
    }
};

/** @brief texture of constant color for 3D entity */
using ImageColor3D = ImageColor<Dimension{3}>;

} // namespace cpp_raytracing

#endif
