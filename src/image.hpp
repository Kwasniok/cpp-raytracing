/**
 * @file
 * @brief image representation
 */

#ifndef CPP_RAYTRACING_IMAGE_H
#define CPP_RAYTRACING_IMAGE_H

#include <iostream>
#include <utility>

#include "color.hpp"

namespace ray {

/**
 * @brief raw image with floating-point channels
 */
class RawImage {
  public:
    /** @brief pixel index type */
    using Index = std::pair<std::size_t, std::size_t>;

    /** @brief initialize with dimensions */
    RawImage(unsigned long width, unsigned long height)
        : _pixel_colors(width * height), _width(width), _height(height) {}
    /** @brief move constructor */
    RawImage(RawImage&) = default;
    RawImage(RawImage&&) = default;

    /** @brief width in pixel */
    unsigned long width() const { return _width; }
    /** @brief height in pixel */
    unsigned long height() const { return _height; }

    /** @brief get pixel {x, y} */
    Color operator[](Index xy) const {
        auto [x, y] = xy;
        return _pixel_colors[y * _width + x];
    }
    /** @brief reference to pixel {x, y} */
    Color& operator[](Index xy) {
        auto [x, y] = xy;
        return _pixel_colors[y * _width + x];
    }

    void operator+=(const RawImage& other) {
        for (unsigned long y = 0; y < _height; ++y) {
            for (unsigned long x = 0; x < _width; ++x) {
                auto i = y * _width + x;
                _pixel_colors[i] += other._pixel_colors[i];
            }
        }
    }

    void operator*=(const Scalar fac) {
        for (unsigned long y = 0; y < _height; ++y) {
            for (unsigned long x = 0; x < _width; ++x) {
                auto i = y * _width + x;
                _pixel_colors[i] *= fac;
            }
        }
    }

  private:
    std::vector<Color> _pixel_colors;
    unsigned long _width;
    unsigned long _height;
};

/**
 * @brief write color as space separated ASCII 8-bit RGB channels
 * @param scale factor to multiply each channel's value with
 */
std::ostream& write_color_as_int_triple(std::ostream& os, const Color& color,
                                        const Scalar scale = 1.0) {
    Scalar r = color.r();
    Scalar g = color.g();
    Scalar b = color.b();
    // scale (e.g. 1/samples)
    r *= scale;
    g *= scale;
    b *= scale;
    // gamma correction (raise to the power of 1/gamma)
    r = std::sqrt(r);
    g = std::sqrt(g);
    b = std::sqrt(b);
    // convert to integers
    const Scalar ir = int_from_color_scalar(r);
    const Scalar ig = int_from_color_scalar(g);
    const Scalar ib = int_from_color_scalar(b);
    os << ir << " " << ig << " " << ib;
    return os;
}

/**
 * @brief write raw image in
 * [Portable PixMap file format](https://en.wikipedia.org/wiki/Netpbm)
 * (P3: ASCII 8-bit RGB)
 * @param scale factor to multiply each channel's value with
 */
std::ostream& write_raw_image_ppm(std::ostream& os, const RawImage& image,
                                  const Scalar scale = 1.0) {

    const unsigned long max_color = 255;

    // header
    os << "P3 # ASCII RGB" << std::endl;
    os << image.width() << " " << image.height() << " # width x height"
       << std::endl;
    os << max_color << " # max color value per channel" << std::endl;

    for (long y = image.height() - 1; y >= 0; --y) {
        for (unsigned long x = 0; x < image.width(); ++x) {
            write_color_as_int_triple(os, image[{x, y}], scale);
            os << "   ";
        }
        os << std::endl;
    }
    return os;
}

} // namespace ray

#endif
