#ifndef CPP_RAYTRACING_IMAGE_H
#define CPP_RAYTRACING_IMAGE_H

#include <iostream>
#include <utility>

#include "color.hpp"

namespace ray {

class RawImage {
  public:
    using Index = std::pair<std::size_t, std::size_t>;

    RawImage(unsigned long width, unsigned long height)
        : _pixel_colors(width * height), _width(width), _height(height) {}
    RawImage(RawImage&&) = default;

    unsigned long width() const { return _width; }
    unsigned long height() const { return _height; }

    Color operator[](Index ji) const {
        auto [j, i] = ji;
        return _pixel_colors[j * _width + i];
    }
    Color& operator[](Index ji) {
        auto [j, i] = ji;
        return _pixel_colors[j * _width + i];
    }

  private:
    std::vector<Color> _pixel_colors;
    unsigned long _width;
    unsigned long _height;
};

std::ostream& write_color_as_int_triple(std::ostream& os, const Color& color) {
    auto r = color.r();
    auto g = color.g();
    auto b = color.b();
    // gamma correction (raise to the power of 1/gamma)
    r = std::sqrt(r);
    g = std::sqrt(g);
    b = std::sqrt(b);
    // convert to integers
    const auto ir = int_from_color_scalar(r);
    const auto ig = int_from_color_scalar(g);
    const auto ib = int_from_color_scalar(b);
    os << ir << " " << ig << " " << ib;
    return os;
}

std::ostream& write_raw_image_ppm(std::ostream& os, const RawImage& image) {

    const unsigned long max_color = 255;

    // header
    os << "P3 # ASCII RGB" << std::endl;
    os << image.width() << " " << image.height() << " # width x height"
       << std::endl;
    os << max_color << " # max color value per channel" << std::endl;

    for (long j = image.height() - 1; j >= 0; --j) {
        for (unsigned long i = 0; i < image.width(); ++i) {
            write_color_as_int_triple(os, image[{j, i}]);
            os << "   ";
        }
        os << std::endl;
    }
    return os;
}

} // namespace ray

#endif
