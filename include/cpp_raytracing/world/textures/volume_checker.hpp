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
 * @note The value at the edges of each cell is undefined.
 */
template <Dimension DIMENSION>
class VolumeChecker : public Texture<DIMENSION> {
  public:
    /** @brief primary color of the surface */
    Color color1 = Colors::WHITE;
    /** @brief secondary color of the surface */
    Color color2 = Colors::BLACK;
    /** @brief offset */
    Vec<DIMENSION> offset{};
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
                const Vec<DIMENSION>& point) const override {

        const auto pos = (point - offset) * (Scalar{1} / scale);

        const auto flip = [](const bool b, const Scalar x) {
            return b ^ (x < 0) ^ (std::fmod(std::abs(x), 2.0) > 1.0);
        };
        const bool is_primary =
            std::accumulate(std::begin(pos), std::end(pos), true, flip);

        return is_primary ? color1 : color2;
    }
};

/** @brief 3D volume checker texture for 3D entities */
using VolumeChecker3D = VolumeChecker<Dimension{3}>;

} // namespace cpp_raytracing

#endif
