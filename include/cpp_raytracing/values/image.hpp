/**
 * @file
 * @brief 2D colored image representation
 */

#ifndef CPP_RAYTRACING_IMAGE2D_HPP
#define CPP_RAYTRACING_IMAGE2D_HPP

#include <cmath>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "./color.hpp"

namespace cpp_raytracing {

/**
 * @brief raw image with floating-point channels
 */
class Image2D {
  public:
    /** @brief pixel index type */
    using Index = std::pair<std::size_t, std::size_t>;

    /** @brief initialize with dimensions */
    Image2D(unsigned long width = 1, unsigned long height = 1)
        : _pixel_colors(width * height), _width(width), _height(height) {}

    /** @brief copy constructor */
    Image2D(const Image2D&) = delete;

    /** @brief move constructor */
    Image2D(Image2D&&) = default;

    /** @brief copy assignment */
    Image2D& operator=(const Image2D&) = delete;

    /** @brief move assignment */
    Image2D& operator=(Image2D&&) = default;

    ~Image2D() = default;

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

    /** @brief const iterator for first element */
    constexpr auto begin() const { return std::cbegin(_pixel_colors); }
    /** @brief const iterator for end */
    constexpr auto end() const { return std::cend(_pixel_colors); }
    /** @brief iterator for first element */
    constexpr auto begin() { return std::begin(_pixel_colors); }
    /** @brief iterator for end */
    constexpr auto end() { return std::end(_pixel_colors); }

    /** @brief add another image pixelwise */
    void operator+=(const Image2D& other) {
        if (_width != other._width || _height != other._height) {
            throw std::runtime_error(
                "Error: Cannot add images of different dimensions.");
        }
        for (unsigned long y = 0; y < _height; ++y) {
            for (unsigned long x = 0; x < _width; ++x) {
                auto i = y * _width + x;
                _pixel_colors[i] += other._pixel_colors[i];
            }
        }
    }

    /** @brief multiply image pixelwise */
    void operator*=(const ColorScalar fac) {
        for (auto& c : _pixel_colors) {
            c *= fac;
        }
    }

    /** @brief initialize with all pixels set to random colors */
    inline static Image2D random(unsigned long width, unsigned long height,
                                 ColorScalar min, ColorScalar max) {
        Image2D img(width, height);
        for (auto& c : img) {
            c = Color::random(min, max);
        }
        return img;
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
 * @param gamma (optional) gamma correction
 * @see read_color_from_ascii_triple
 */
void write_color_as_ascii_triple(std::ostream& os, const Color& color,
                                 const ColorScalar scale = 1.0,
                                 const ColorScalar gamma = 1.0,
                                 const ColorIntegral max_value = 255) {
    ColorScalar r = color.r();
    ColorScalar g = color.g();
    ColorScalar b = color.b();
    // scale (e.g. 1/samples)
    r *= scale;
    g *= scale;
    b *= scale;
    // gamma correction
    r = std::pow(r, 1 / gamma);
    g = std::pow(g, 1 / gamma);
    b = std::pow(b, 1 / gamma);
    // convert to integers
    const ColorIntegral ir = int_from_color_scalar(r, max_value);
    const ColorIntegral ig = int_from_color_scalar(g, max_value);
    const ColorIntegral ib = int_from_color_scalar(b, max_value);
    // write
    os << ir << " " << ig << " " << ib;
}

/**
 * @brief read color as space separated ASCII 8-bit RGB channels
 * @param is input stream
 * @param color color to be written
 * @param scale (optinal) factor to divide each channel's value by
 * @param gamma (optional) gamma correction
 * @see write_color_as_ascii_triple
 */
Color read_color_from_ascii_triple(std::istream& is,
                                   const ColorScalar scale = 1.0,
                                   const ColorScalar gamma = 1.0,
                                   const ColorIntegral max_value = 255) {

    ColorIntegral ir, ig, ib;
    is >> ir >> ig >> ib;
    // convert to scalars
    ColorScalar r = color_scalar_from_int(ir, max_value);
    ColorScalar g = color_scalar_from_int(ig, max_value);
    ColorScalar b = color_scalar_from_int(ib, max_value);
    // gamma correction
    r = std::pow(r, gamma);
    g = std::pow(g, gamma);
    b = std::pow(b, gamma);
    // scale (e.g. 1/samples)
    r /= scale;
    g /= scale;
    b /= scale;
    // return
    return {r, g, b};
}

/**
 * @brief write image in
 * [Portable PixMap file format](https://en.wikipedia.org/wiki/Netpbm)
 * (P3: ASCII 8-bit RGB)
 * @note Includes gamma correction of `gamma = 0.5`.
 * @param os output stream
 * @param image image to be written
 * @param scale (optional) factor to multiply each channel's value with
 * @param gamma (optional) gamma correction
 * @see read_image_ppm
 */
void write_image_ppm(std::ostream& os, const Image2D& image,
                     const ColorScalar scale = 1.0,
                     const ColorScalar gamma = 1.0,
                     const ColorIntegral max_color = 255) {

    // header
    os << "P3" << std::endl;
    os << image.width() << " " << image.height() << std::endl;
    os << max_color << std::endl;

    // body
    for (unsigned long y = image.height() - 1;
         y != std::numeric_limits<unsigned long>::max(); --y) {
        for (unsigned long x = 0; x < image.width(); ++x) {
            write_color_as_ascii_triple(os, image[{x, y}], scale, gamma,
                                        max_color);
            os << "   ";
        }
        os << std::endl;
    }
}

/**
 * @brief read image from
 * [Portable PixMap file format](https://en.wikipedia.org/wiki/Netpbm)
 * (P3: ASCII 8-bit RGB)
 * @note Includes gamma correction of `gamma = 0.5`.
 * @note Format requirmenets are very strict: E.g. No comments or extra white
 * spaces allowed.
 * @param is input stream
 * @param image image to be read
 * @param scale (optional) factor to divide each channel's value by
 * @param gamma (optional) gamma correction
 * @see write_image_ppm
 */
Image2D read_image_ppm(std::istream& is, const ColorScalar scale = 1.0,
                       const ColorScalar gamma = 1.0) {
    // header
    // P3
    std::string magic_number;
    is >> magic_number;
    if (!is) {
        throw std::runtime_error("Error: Cannot read file. File not in Netpbm "
                                 "P3 format. Could not read magic number.");
    }
    if (magic_number != "P3") {
        throw std::runtime_error("Error: Cannot read file. File not in Netpbm "
                                 "P3 format. Unexpected magic number.");
    }
    // width height
    std::size_t width, height;
    is >> width >> height;
    if (!is) {
        throw std::runtime_error("Error: Cannot read file. File not in Netpbm "
                                 "P3 format. Could not read width and height.");
    }
    if ((width < 1) || (height < 1)) {
        throw std::runtime_error(
            "Error: Cannot read file. File not in Netpbm "
            "P3 format. Width or height must be positive.");
    }
    // max color value
    ColorIntegral max_color;
    is >> max_color;
    if (!is) {
        throw std::runtime_error("Error: Cannot read file. File not in Netpbm "
                                 "P3 format. Could not read maximum value.");
    }
    if (max_color < 1) {
        throw std::runtime_error(
            "Error: Cannot read file. File not in Netpbm "
            "P3 format. Unexpected maximum color value (expected >= 1).");
    }

    // body
    Image2D image(width, height);
    for (unsigned long y = height - 1;
         y != std::numeric_limits<unsigned long>::max(); --y) {
        for (unsigned long x = 0; x < width; ++x) {
            image[{x, y}] = read_color_from_ascii_triple(is, scale, gamma, max_color);
            if (!is) {
                throw std::runtime_error(
                    "Error: Cannot read file. File not in Netpbm "
                    "P3 format. Could not read color triplet.");
            }
        }
    }

    // tail
    is >> std::ws;
    if (!is.eof()) {
        throw std::runtime_error(
            "Error: Cannot read file. File not in Netpbm "
            "P3 format. File did not end after last color triplet.");
    }

    return image;
}

/** @brief write binary 32-bit little-endian float to stream */
void write_binary_float32(std::ostream& os, const float value) {
    // convert to bytes (asserts IEEE 754 32-bit little endian)
    constexpr static auto IEEE_754_DIGITS = 24;
    static_assert(std::numeric_limits<float>::is_iec559,
                  "float is not IEEE 754 conform");
    static_assert(std::numeric_limits<float>::digits == IEEE_754_DIGITS,
                  "float is not single (32-bit) IEEE 754 conform");
    static_assert(std::endian::native == std::endian::little,
                  "float is not little-endian");
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast): see above
    const char* const binary_data = reinterpret_cast<const char*>(&value);

    // write bytes
    os.write(binary_data, sizeof(value));
}

/**
 * @brief write color as space separated binary 32-bit little-endian float RGB
 *        channels
 * @note No gamma correction.
 * @param os output stream
 * @param color color to be written
 */
void write_color_as_float32_triple(std::ostream& os, const Color& color) {
    // convert to float32
    const float fr = static_cast<float>(color.r());
    const float fg = static_cast<float>(color.g());
    const float fb = static_cast<float>(color.b());

    // no gamma correction

    // write bytes
    write_binary_float32(os, fr);
    write_binary_float32(os, fg);
    write_binary_float32(os, fb);
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
void write_image_pfm(std::ostream& os, const Image2D& image,
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
}

} // namespace cpp_raytracing

#endif
