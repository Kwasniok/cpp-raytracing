#ifndef CPP_RAYTRACING_COLOR_H
#define CPP_RAYTRACING_COLOR_H

#include <iostream>

#include "util.hpp"

namespace ray {

using ColorScalar = double;
using ColorIntegral = unsigned long;

class Color {
  public:
    Color() : _data{0, 0, 0} {}
    explicit Color(const ColorScalar r, const ColorScalar g,
                   const ColorScalar b)
        : _data{r, g, b} {}

    ColorScalar r() const { return _data[0]; }
    ColorScalar g() const { return _data[1]; }
    ColorScalar b() const { return _data[2]; }

    Color operator-() const { return Color{-_data[0], _data[1], -_data[2]}; }
    ColorScalar operator[](unsigned long i) const { return _data[i]; }
    ColorScalar& operator[](unsigned long i) { return _data[i]; }
    Color& operator+=(const Color& other) {
        _data[0] += other._data[0];
        _data[1] += other._data[1];
        _data[2] += other._data[2];
        return *this;
    }
    Color& operator-=(const Color& other) { return *this += (-other); }
    Color& operator*=(const ColorScalar fac) {
        _data[0] *= fac;
        _data[1] *= fac;
        _data[2] *= fac;
        return *this;
    }
    Color& operator/=(const ColorScalar fac) { return *this *= (1 / fac); }

  private:
    ColorScalar _data[3];
};

inline ColorIntegral integral_from_color_scalar(const ColorScalar cs) {
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

inline Color operator+(const Color& color1, const Color& color2) {
    return Color(color1.r() + color2.r(), color1.g() + color2.g(),
                 color1.b() + color2.b());
}

inline Color operator-(const Color& color1, const Color& color2) {
    return Color(color1.r() - color2.r(), color1.g() - color2.g(),
                 color1.b() - color2.b());
}

inline Color operator*(const Color& color, const ColorScalar f) {
    return Color(color.r() * f, color.g() * f, color.b() * f);
}

inline Color operator*(const ColorScalar f, const Color& color) {
    return color * f;
}

inline Color operator/(const Color& color, const ColorScalar f) {
    const ColorScalar f_inv = 1 / f;
    return Color(color.r() * f_inv, color.g() * f_inv, color.b() * f_inv);
}

} // namespace ray
#endif
