/**
 * @file
 * @brief color representation
 */

#ifndef CPP_RAYTRACING_COLOR_HPP
#define CPP_RAYTRACING_COLOR_HPP

#include <iostream>

#include "util.hpp"

namespace cpp_raytracing {

/** @brief floating-point value per color channel */
using ColorScalar = double;
/** @brief fixed-point value per color channel */
using ColorIntegral = unsigned long;

/**
 * @brief represents the tree color channels (RGB) with floating-point values
 */
class Color {

  public:
    /**
     * @brief initialize all channels as zeros
     * @see Colors::BLACK
     */
    constexpr Color() : _data{0.0, 0.0, 0.0} {}
    /** @brief initialize with RGB channel values */
    constexpr Color(const ColorScalar r, const ColorScalar g,
                    const ColorScalar b)
        : _data{r, g, b} {}
    /** @brief copy constructor */
    constexpr Color(const Color& other)
        : _data{
              other.r(),
              other.g(),
              other.b(),
          } {}
    /** @brief red channel*/
    constexpr ColorScalar r() const { return _data[0]; }
    /** @brief green channel*/
    constexpr ColorScalar g() const { return _data[1]; }
    /** @brief blue channel*/
    constexpr ColorScalar b() const { return _data[2]; }

    /** @brief tests equivalence */
    constexpr bool operator==(const Color& other) const {
        return r() == other.r() && g() == other.g() && b() == other.b();
    }

    /** @brief tests inequivalence */
    constexpr bool operator!=(const Color& other) const {
        return r() != other.r() || g() != other.g() || b() != other.b();
    }

    /** @brief negate channelwise */
    constexpr Color operator-() const {
        return Color{-_data[0], -_data[1], -_data[2]};
    }
    /**
     * @brief enumerated channel access
     * @note 0=red, 1=green, 2=blue
     */
    constexpr ColorScalar operator[](unsigned long i) const { return _data[i]; }
    /**
     * @brief enumerated channel access
     * @note 0=red, 1=green, 2=blue
     */
    constexpr ColorScalar& operator[](unsigned long i) { return _data[i]; }
    /** @brief add channelwise */
    constexpr Color& operator+=(const Color& other) {
        _data[0] += other._data[0];
        _data[1] += other._data[1];
        _data[2] += other._data[2];
        return *this;
    }
    /** @brief subtract channelwise */
    constexpr Color& operator-=(const Color& other) {
        return *this += (-other);
    }
    /** @brief multiply channelwise */
    constexpr Color& operator*=(const ColorScalar fac) {
        _data[0] *= fac;
        _data[1] *= fac;
        _data[2] *= fac;
        return *this;
    }
    /** @brief multiply channelwise */
    constexpr Color& operator*=(const Color& other) {
        _data[0] *= other[0];
        _data[1] *= other[1];
        _data[2] *= other[2];
        return *this;
    }
    /** @brief divide channelwise */
    constexpr Color& operator/=(const ColorScalar fac) {
        return *this *= (1 / fac);
    }

  private:
    ColorScalar _data[3];
};

/** @brief write Color to stream */
inline std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << "Color(" << color.r() << ", " << color.g() << ", " << color.b()
       << ")";
    return os;
}

/**
 * @brief convert floating point color value to integer in range 0-255
 * channelwise
 * @note cs > 1.0 infinity and NaN clip to 255
 * @note cs < 0.0 -infinity clip to 0
 */
inline constexpr ColorIntegral int_from_color_scalar(const ColorScalar cs) {
    ColorIntegral ci = static_cast<ColorIntegral>(cs * 255);
    return clip<ColorIntegral, 0, 255>(ci);
}

/** @brief add channelwise */
inline constexpr Color operator+(const Color& color1, const Color& color2) {
    return Color(color1.r() + color2.r(), color1.g() + color2.g(),
                 color1.b() + color2.b());
}

/** @brief subtract channelwise */
inline constexpr Color operator-(const Color& color1, const Color& color2) {
    return Color(color1.r() - color2.r(), color1.g() - color2.g(),
                 color1.b() - color2.b());
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const Color& color, const ColorScalar f) {
    return Color(color.r() * f, color.g() * f, color.b() * f);
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const Color& color1, const Color& color2) {
    return Color(color1.r() * color2.r(), color1.g() * color2.g(),
                 color1.b() * color2.b());
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const ColorScalar f, const Color& color) {
    return color * f;
}

/** @brief divide channelwise */
inline constexpr Color operator/(const Color& color, const ColorScalar f) {
    const ColorScalar f_inv = 1 / f;
    return Color(color.r() * f_inv, color.g() * f_inv, color.b() * f_inv);
}

/** @brief color constants */
namespace Colors {
constexpr Color BLACK{0.0, 0.0, 0.0}; /**< @brief black */
constexpr Color WHITE{1.0, 1.0, 1.0}; /**< @brief white */
constexpr Color RED{1.0, 0.0, 0.0};   /**< @brief red */
constexpr Color GREEN{0.0, 1.0, 0.0}; /**< @brief green */
constexpr Color BLUE{0.0, 0.0, 1.0};  /**< @brief blue */
} // namespace Colors

} // namespace cpp_raytracing
#endif
