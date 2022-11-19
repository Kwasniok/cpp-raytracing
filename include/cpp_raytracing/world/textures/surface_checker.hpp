/**
 * @file
 * @brief UV coordinates-based checker texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP
#define CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP

#include <algorithm>
#include <cmath>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple UV coordinates-based checker texture
 * @note The value at the edges of each cell is undefined.
 */
template <Dimension DIMENSION>
class SurfaceChecker : public Texture<DIMENSION> {
  public:
    /** @brief primary color of the surface */
    Color color1 = Colors::WHITE;
    /** @brief secondary color of the surface */
    Color color2 = Colors::BLACK;
    /** @brief offset */
    Vec2 offset = Vec2{0.0, 0.0};
    /** @brief size of each box in uv coordinates */
    Scalar scale = 1.0;

    /** @brief default construct with default idenfifier root */
    SurfaceChecker() = default;

    /** @brief copy constructor */
    SurfaceChecker(const SurfaceChecker&) = delete;

    /** @brief move constructor */
    SurfaceChecker(SurfaceChecker&&) = default;

    /** @brief copy assignment */
    SurfaceChecker& operator=(const SurfaceChecker&) = delete;

    /** @brief move assignment */
    SurfaceChecker& operator=(SurfaceChecker&&) = default;

    ~SurfaceChecker() override = default;

    /** @see Texture::value */
    Color value(const Vec2& uv_coordinates,
                [[maybe_unused]] const Vec<DIMENSION>& point) const override {

        const Vec2 coords = (uv_coordinates - offset) * (Scalar{1} / scale);

        const auto flip = [](const bool b, const Scalar x) {
            return b ^ (x < 0) ^ (std::fmod(std::abs(x), 2.0) > 1.0);
        };
        const bool is_primary =
            std::accumulate(std::begin(coords), std::end(coords), true, flip);

        return is_primary ? color1 : color2;
    }
};

/** @brief 2D surface checker texture  for 3D entities */
using SurfaceChecker3D = SurfaceChecker<Dimension{3}>;

} // namespace cpp_raytracing

#endif
