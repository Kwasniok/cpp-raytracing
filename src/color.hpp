#ifndef CPP_RAYTRACING_COLOR_H
#define CPP_RAYTRACING_COLOR_H

#include <iostream>

#include "util.hpp"

namespace ray {

using ColorScalar = double;
using ColorIntegral = unsigned long;

class Color {
  public:
    constexpr Color() : _data{0, 0, 0} {}
    constexpr Color(const ColorScalar r, const ColorScalar g,
                    const ColorScalar b)
        : _data{r, g, b} {}
    constexpr Color(const Color& other)
        : _data{
              other.r(),
              other.g(),
              other.b(),
          } {}

    constexpr ColorScalar r() const { return _data[0]; }
    constexpr ColorScalar g() const { return _data[1]; }
    constexpr ColorScalar b() const { return _data[2]; }

    constexpr Color operator-() const {
        return Color{-_data[0], _data[1], -_data[2]};
    }
    constexpr ColorScalar operator[](unsigned long i) const { return _data[i]; }
    constexpr ColorScalar& operator[](unsigned long i) { return _data[i]; }
    constexpr Color& operator+=(const Color& other) {
        _data[0] += other._data[0];
        _data[1] += other._data[1];
        _data[2] += other._data[2];
        return *this;
    }
    constexpr Color& operator-=(const Color& other) {
        return *this += (-other);
    }
    constexpr Color& operator*=(const ColorScalar fac) {
        _data[0] *= fac;
        _data[1] *= fac;
        _data[2] *= fac;
        return *this;
    }
    constexpr Color& operator/=(const ColorScalar fac) {
        return *this *= (1 / fac);
    }

  private:
    ColorScalar _data[3];
};

inline constexpr ColorIntegral
integral_from_color_scalar(const ColorScalar cs) {
    ColorIntegral ci = static_cast<ColorIntegral>(cs * 255);
    return clip<ColorIntegral, 0, 255>(ci);
}

inline std::ostream& write_color_as_integrals(std::ostream& os,
                                              const Color color) {
    const unsigned r = integral_from_color_scalar(color.r());
    const unsigned g = integral_from_color_scalar(color.g());
    const unsigned b = integral_from_color_scalar(color.b());
    os << r << " " << g << " " << b;
    return os;
}

inline constexpr Color operator+(const Color& color1, const Color& color2) {
    return Color(color1.r() + color2.r(), color1.g() + color2.g(),
                 color1.b() + color2.b());
}

inline constexpr Color operator-(const Color& color1, const Color& color2) {
    return Color(color1.r() - color2.r(), color1.g() - color2.g(),
                 color1.b() - color2.b());
}

inline constexpr Color operator*(const Color& color, const ColorScalar f) {
    return Color(color.r() * f, color.g() * f, color.b() * f);
}

inline constexpr Color operator*(const ColorScalar f, const Color& color) {
    return color * f;
}

inline constexpr Color operator/(const Color& color, const ColorScalar f) {
    const ColorScalar f_inv = 1 / f;
    return Color(color.r() * f_inv, color.g() * f_inv, color.b() * f_inv);
}

} // namespace ray
#endif
