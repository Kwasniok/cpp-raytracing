/**
 * @file
 * @brief color representation
 */

#ifndef CPP_RAYTRACING_COLOR_HPP
#define CPP_RAYTRACING_COLOR_HPP

#include <iostream>

#include <gttl/tensor.hpp>

#include "../util.hpp"
#include "../values/random.hpp"

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
    constexpr Color() : _data{} {}
    /** @brief initialize with RGB channel values */
    constexpr Color(const ColorScalar r, const ColorScalar g,
                    const ColorScalar b)
        : _data{r, g, b} {}

    /** @brief initialize channels with random values */
    inline static Color random(Scalar min, Scalar max) {
        return {random_scalar(min, max), random_scalar(min, max),
                random_scalar(min, max)};
    }

    /** @brief red channel*/
    constexpr ColorScalar r() const { return _data[0]; }
    /** @brief green channel*/
    constexpr ColorScalar g() const { return _data[1]; }
    /** @brief blue channel*/
    constexpr ColorScalar b() const { return _data[2]; }

    /** @brief red channel*/
    constexpr ColorScalar& r() { return _data[0]; }
    /** @brief green channel*/
    constexpr ColorScalar& g() { return _data[1]; }
    /** @brief blue channel*/
    constexpr ColorScalar& b() { return _data[2]; }

    /** @brief const iterator for first element */
    constexpr auto begin() const { return std::cbegin(_data); }
    /** @brief const iterator for end */
    constexpr auto end() const { return std::cend(_data); }
    /** @brief iterator for first element */
    constexpr auto begin() { return std::begin(_data); }
    /** @brief iterator for end */
    constexpr auto end() { return std::end(_data); }

    /** @brief negate channelwise */
    constexpr Color operator-() const { return Color{-_data}; }

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
        _data += other._data;
        return *this;
    }
    /** @brief subtract channelwise */
    constexpr Color& operator-=(const Color& other) {
        _data -= other._data;
        return *this;
    }
    /** @brief multiply channelwise */
    constexpr Color& operator*=(const ColorScalar fac) {
        _data *= fac;
        return *this;
    }
    /** @brief multiply channelwise */
    constexpr Color& operator*=(const Color& other) {
        _data.inplace_elementwise(std::multiplies<ColorScalar>{}, other._data);
        return *this;
    }
    /** @brief divide channelwise */
    constexpr Color& operator/=(const ColorScalar fac) {
        _data *= ColorScalar{1} / fac;
        return *this;
    }

    friend constexpr Color operator+(const Color&, const Color&);
    friend constexpr Color operator-(const Color&, const Color&);
    friend constexpr Color operator*(const Color&, const ColorScalar);
    friend constexpr Color operator*(const Color&, const Color&);
    friend constexpr Color operator*(const ColorScalar, const Color&);
    friend constexpr Color operator/(const Color&, const ColorScalar);

  private:
    using data_type = gttl::Tensor<ColorScalar, 1, gttl::Dimensions<1>{3}>;
    constexpr Color(const data_type& data) : _data(data) {}

    data_type _data;
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
inline constexpr ColorIntegral int_from_color_scalar(ColorScalar cs) {
    if (std::isnan(cs)) {
        return 0;
    }
    cs = clip(cs, 0.0, 1.0);
    const ColorIntegral ci = static_cast<ColorIntegral>(cs * 255.0);
    return clip(ci, static_cast<ColorIntegral>(0),
                static_cast<ColorIntegral>(255));
}

/** @brief add channelwise */
inline constexpr Color operator+(const Color& color1, const Color& color2) {
    return Color(color1._data + color2._data);
}

/** @brief subtract channelwise */
inline constexpr Color operator-(const Color& color1, const Color& color2) {
    return Color(color1._data - color2._data);
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const Color& color, const ColorScalar f) {
    return Color(color._data * f);
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const Color& color1, const Color& color2) {
    const auto mul = typename Color::data_type::traits_type::mul{};
    return Color(color1._data.elementwise(mul, color2._data));
}

/** @brief multiply channelwise */
inline constexpr Color operator*(const ColorScalar f, const Color& color) {
    return color * f;
}

/** @brief divide channelwise */
inline constexpr Color operator/(const Color& color, const ColorScalar f) {
    const auto div = typename Color::data_type::traits_type::div{};
    const auto func = [&](const auto x) constexpr {
        return div(x, f);
    };
    return Color(color._data.elementwise(func));
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
