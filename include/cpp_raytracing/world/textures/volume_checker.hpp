/**
 * @file
 * @brief manifold coordinates-based checker texture
 */

#ifndef CPP_RAYTRACING_TEXTURES_CHECKER_3D_HPP
#define CPP_RAYTRACING_TEXTURES_CHECKER_3D_HPP

#include <cmath>
#include <functional>
#include <numeric>

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple manifold coordinates-based checker texture
 */
template <Dimension DIMENSION>
class VolumeChecker : public Texture<DIMENSION> {
  public:
    /** @brief volume vector type */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief primary color of the surface */
    Color color1 = Colors::WHITE;
    /** @brief secondary color of the surface */
    Color color2 = Colors::BLACK;
    /** @brief offset */
    Vec3 offset = Vec3{0.0, 0.0, 0.0};
    /** @brief size of each box in uv coordinates */
    Scalar scale = 1.0;
    /** @brief default construct with default idenfifier root */
    VolumeChecker() = default;

    /** @brief copy constructor */
    VolumeChecker(const VolumeChecker&) = delete;

    /** @brief move constructor */
    VolumeChecker(VolumeChecker&&) = default;

    /** @brief copy assignment */
    VolumeChecker& operator=(const VolumeChecker&) = delete;

    /** @brief move assignment */
    VolumeChecker& operator=(VolumeChecker&&) = default;

    ~VolumeChecker() override = default;

    /** @see Texture::value */
    Color value([[maybe_unused]] const Vec2& uv_coordinates,
                const VolumeVec& point) const override {

        const VolumeVec p = (point - offset) * (pi / scale);
        const VolumeVec sins =
            p.elementwise([](const Scalar x) { return std::sin(x); });

        const auto val = std::accumulate(std::begin(sins), std::end(sins),
                                         Scalar{1}, std::multiplies{});

        const bool is_primary = val < 0.0;

        return is_primary ? color1 : color2;
    }
};

/** @brief 3D volume checker texture for 3D entities */
using VolumeChecker3D = VolumeChecker<Dimension{3}>;

} // namespace cpp_raytracing

#endif
