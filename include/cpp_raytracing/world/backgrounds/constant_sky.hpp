/**
 * @file
 * @brief constant sky background
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_CONSTANT_SKY_HPP
#define CPP_RAYTRACING_BACKGROUNDS_CONSTANT_SKY_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief constant sky background
 */
class ConstantSky : public Background {
  public:
    /** @brief color of the sky */
    Color color = Colors::WHITE;

    virtual ~ConstantSky() = default;

    virtual Color value(const Vec3& direction) const override { return color; };
};

} // namespace cpp_raytracing

#endif
