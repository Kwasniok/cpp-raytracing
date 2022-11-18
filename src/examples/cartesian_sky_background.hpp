/**
 * @file
 * @brief simple (Cartesian euclidean) sky background
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_CARTESIAN_SKY_BACKGROUNDS_HPP
#define CPP_RAYTRACING_BACKGROUNDS_CARTESIAN_SKY_BACKGROUNDS_HPP

#include <cpp_raytracing/world/backgrounds/base.hpp>

namespace cpp_raytracing {

/**
 * @brief simple sky background (requires Cartesian coorinates)
 */
template <Dimension DIMENSION>
class CartesianSkyBackground : public Background<DIMENSION> {
  public:
    /** @brief color near horizon */
    Color color1 = Colors::WHITE;
    /** @brief color near horizon */
    Color color2 = {0.5, 0.7, 1.0};

    /** @brief default constructor */
    CartesianSkyBackground() = default;

    /** @brief copy constructor */
    CartesianSkyBackground(const CartesianSkyBackground&) = delete;

    /** @brief move constructor */
    CartesianSkyBackground(CartesianSkyBackground&&) = default;

    /** @brief copy assignment */
    CartesianSkyBackground& operator=(const CartesianSkyBackground&) = delete;

    /** @brief move assignment */
    CartesianSkyBackground& operator=(CartesianSkyBackground&&) = default;

    ~CartesianSkyBackground() override = default;

    /** @see Background::value */
    Color value([[maybe_unused]] const Geometry<DIMENSION>& geometry,
                const RaySegment<DIMENSION>& ray_segment) const override {

        using namespace tensor;

        const auto direction = unit_vector(ray_segment.direction());
        const auto t = 0.5 * (std::abs(direction[1]) + 1.0);
        const Color color = (1.0 - t) * color1 + t * color2;
        return color;
    };
};

/** @brief sky background for R^3 */
using CartesianSkyBackground3D = CartesianSkyBackground<Dimension{3}>;

} // namespace cpp_raytracing

#endif
