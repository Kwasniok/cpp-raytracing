/**
 * @file
 * @brief simple interpolated sky background
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_SIMPLE_SKY_HPP
#define CPP_RAYTRACING_BACKGROUNDS_SIMPLE_SKY_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple interpolated sky background
 */
class SimpleSky : public Background {
  public:
    /** @brief color close to the horizon */
    Color color1 = Colors::WHITE;
    /** @brief color close to the zenith/azimuth */
    Color color2 = {0.5, 0.7, 1.0};

    virtual ~SimpleSky() = default;

    /** @brief interpolation of primary and secondary color */
    virtual Color value(const Vec3& direction) const override {
        const auto dir = unit_vector(direction);
        const auto t = 0.5 * (std::abs(dir.y()) + 1.0);
        return (1.0 - t) * color1 + t * color2;
    };
};

} // namespace cpp_raytracing

#endif
