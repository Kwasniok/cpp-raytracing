/**
 * @file
 * @brief UV coordinates-based checker texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP
#define CPP_RAYTRACING_TEXTURES_CHECKER_2D_HPP

#include <cmath>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple UV coordinates-based checker texture
 */
template <Dimension DIMENSION>
class SurfaceChecker : public Texture<DIMENSION> {
  public:
    /** @brief volume vector type */
    using VolumeVec = Vec<DIMENSION>;

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
                [[maybe_unused]] const VolumeVec& point) const override {

        const Vec2 c = (uv_coordinates - offset) * (pi / scale);
        const auto val = std::sin(c[0]) * std::sin(c[1]);

        const bool is_primary = val < 0.0;

        return is_primary ? color1 : color2;
    }
};

/** @brief 2D surface checker texture  for 3D entities */
using SurfaceChecker3D = SurfaceChecker<Dimension{3}>;

} // namespace cpp_raytracing

#endif
