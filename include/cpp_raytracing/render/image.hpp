/**
 * @file
 * @brief image representation
 */

#ifndef CPP_RAYTRACING_IMAGE_HPP
#define CPP_RAYTRACING_IMAGE_HPP

#include <cmath>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "../values/color.hpp"

namespace cpp_raytracing {

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

    /** @brief copy constructor */
    RawImage(const RawImage&) = delete;

    /** @brief move constructor */
    RawImage(RawImage&&) = default;

    /** @brief copy assignment */
    RawImage& operator=(const RawImage&) = delete;

    /** @brief move assignment */
    RawImage& operator=(RawImage&&) = default;

    ~RawImage() = default;

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

    /** @brief add another image pixelwise */
    void operator+=(const RawImage& other) {
        for (unsigned long y = 0; y < _height; ++y) {
            for (unsigned long x = 0; x < _width; ++x) {
                auto i = y * _width + x;
                _pixel_colors[i] += other._pixel_colors[i];
            }
        }
    }

    /** @brief multiply image pixelwise */
    void operator*=(const ColorScalar fac) {
        for (unsigned long y = 0; y < _height; ++y) {
            for (unsigned long x = 0; x < _width; ++x) {
                auto i = y * _width + x;
                _pixel_colors[i] *= fac;
            }
        }
    }

    /** @brief set all pixels to random colors */
    void randomize(Scalar min, Scalar max) {
        for (unsigned long j = 0; j < _height; ++j) {
            for (unsigned long i = 0; i < _width; ++i) {
                this->operator[]({i, j}) += Color::random(min, max);
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
 * @param os output stream
 * @param color color to be written
 * @param scale (optinal) factor to multiply each channel's value with
 * @param inv_gamma  `inv_gamma = 1.0 / gamma` for gamma correction
 */
std::ostream& write_color_as_uint8_triple(std::ostream& os, const Color& color,
                                          const ColorScalar scale = 1.0,
                                          const ColorScalar inv_gamma = 1.0) {
    ColorScalar r = color.r();
    ColorScalar g = color.g();
    ColorScalar b = color.b();
    // scale (e.g. 1/samples)
    r *= scale;
    g *= scale;
    b *= scale;
    // gamma correction
    r = std::pow(r, inv_gamma);
    g = std::pow(g, inv_gamma);
    b = std::pow(b, inv_gamma);
    // convert to integers
    const ColorIntegral ir = int_from_color_scalar(r);
    const ColorIntegral ig = int_from_color_scalar(g);
    const ColorIntegral ib = int_from_color_scalar(b);
    os << ir << " " << ig << " " << ib;
    return os;
}

/**
 * @brief write image in
 * [Portable PixMap file format](https://en.wikipedia.org/wiki/Netpbm)
 * (P3: ASCII 8-bit RGB)
 * @note Includes gamma correction of `gamma = 0.5`.
 * @param os output stream
 * @param image image to be written
 * @param scale (optional) factor to multiply each channel's value with
 * @param gamma gamma correction
 */
std::ostream& write_image_ppm(std::ostream& os, const RawImage& image,
                              const ColorScalar scale = 1.0,
                              const ColorScalar gamma = 1.0) {

    const ColorIntegral max_color = 255;

    // header
    os << "P3 # ASCII 8-bit RGB" << std::endl;
    os << image.width() << " " << image.height() << " # width x height"
       << std::endl;
    os << max_color << " # max color value per channel" << std::endl;

    for (long y = image.height() - 1; y >= 0; --y) {
        for (unsigned long x = 0; x < image.width(); ++x) {
            write_color_as_uint8_triple(os, image[{x, y}], scale, 1.0 / gamma);
            os << "   ";
        }
        os << std::endl;
    }
    return os;
}

/**
 * @brief write color as space separated binary 32-bit little-endian float RGB
 *        channels
 * @note No gamma correction.
 * @param os output stream
 * @param color color to be written
 */
std::ostream& write_color_as_float32_triple(std::ostream& os,
                                            const Color& color) {
    // convert to float32
    const float fr = static_cast<float>(color.r());
    const float fg = static_cast<float>(color.g());
    const float fb = static_cast<float>(color.b());
    // no gamma correction

    // convert to bytes (asserts IEEE 754 32-bit little endian)
    static_assert(std::numeric_limits<float>::is_iec559,
                  "float is not IEEE 754 conform");
    static_assert(std::numeric_limits<float>::digits == 24,
                  "float is not single (32-bit) IEEE 754 conform");
    static_assert(std::endian::native == std::endian::little,
                  "float is not little-endian");
    const char* const sr = reinterpret_cast<const char*>(&fr);
    const char* const sg = reinterpret_cast<const char*>(&fg);
    const char* const sb = reinterpret_cast<const char*>(&fb);

    // write bytes
    os.write(sr, sizeof(fr));
    os.write(sg, sizeof(fg));
    os.write(sb, sizeof(fb));
    return os;
}

/**
 * @brief write raw image in
 * [Portable FloatMap file format](https://en.wikipedia.org/wiki/Netpbm)
 * (PF: binary 32-bit little-endian float RGB)
 * @note No gamma correction.
 * @note Typically `scale = 1 / samples` for raw images.
 * @param os output stream
 * @param image image to be written
 * @param scale (optional) factor to multiply each channel's value with
 */
std::ostream& write_image_pfm(std::ostream& os, const RawImage& image,
                              const ColorScalar scale = 1.0) {

    // header
    os << "PF\n"; // binary 32-bit float RGB
    os << image.width() << " " << image.height() << '\n'; // # width x height"
    os << "-" << std::abs(scale) << "\n"; // # negative <-> little endian"

    for (unsigned long y = 0; y < image.height(); ++y) {
        for (unsigned long x = 0; x < image.width(); ++x) {
            write_color_as_float32_triple(os, image[{x, y}]);
        }
    }
    return os;
}

} // namespace cpp_raytracing

#endif
