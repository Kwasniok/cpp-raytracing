/**
 * @file
 * @brief constant background
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_CONSTANT_HPP
#define CPP_RAYTRACING_BACKGROUNDS_CONSTANT_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief constant background (global illumination)
 */
template <Dimension DIMENSION>
class ConstantBackground : public Background<DIMENSION> {
  public:
    /** @brief color of the background */
    Color color = Colors::WHITE;

    /** @brief default constructor */
    ConstantBackground() = default;

    /** @brief copy constructor */
    ConstantBackground(const ConstantBackground&) = delete;

    /** @brief move constructor */
    ConstantBackground(ConstantBackground&&) = default;

    /** @brief copy assignment */
    ConstantBackground& operator=(const ConstantBackground&) = delete;

    /** @brief move assignment */
    ConstantBackground& operator=(ConstantBackground&&) = default;

    ~ConstantBackground() override = default;

    Color value([[maybe_unused]] const Geometry& geometry,
                [[maybe_unused]] const RaySegment<DIMENSION>& ray_segment)
        const override {
        return color;
    };
};

/** @brief constant background (global illumination) for 3D manifolds */
using ConstantBackground3D = ConstantBackground<Dimension{3}>;

} // namespace cpp_raytracing

#endif
