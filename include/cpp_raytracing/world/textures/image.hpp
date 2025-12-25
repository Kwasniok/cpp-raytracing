/**
 * @file
 * @brief image texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_IMAGE_HPP
#define CPP_RAYTRACING_TEXTURES_IMAGE_HPP

#include "../../values/image.hpp"

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief image texture
 */
template <Dimension DIMENSION>
class Image2DTexture : public Texture<DIMENSION> {
  public:
    /** @brief image color for the surface */
    std::shared_ptr<Image2D> image;

    Image2DTexture() = default;

    /** @brief copy constructor */
    Image2DTexture(const Image2DTexture&) = delete;

    /** @brief move constructor */
    Image2DTexture(Image2DTexture&&) = default;

    /** @brief copy assignment */
    Image2DTexture& operator=(const Image2DTexture&) = delete;

    /** @brief move assignment */
    Image2DTexture& operator=(Image2DTexture&&) = default;

    ~Image2DTexture() override = default;

    /** @see Texture::value */
    Color value([[maybe_unused]] const Vec2& coordinates,
                [[maybe_unused]] const Vec<DIMENSION>& point) const override {

        if (!image) {
            return this->value_for_missing_texture(coordinates, point);
        }

        const auto [u, v] = coordinates.coefficients;
        const unsigned long i = fmod(u, 1) * image->width();
        const unsigned long j = fmod(1 - v, 1) * image->height();
        return image->operator[]({i, j});
    }
};

/** @brief texture of image color for 3D entity */
using Image2DColor3D = Image2DTexture<Dimension{3}>;

} // namespace cpp_raytracing

#endif
