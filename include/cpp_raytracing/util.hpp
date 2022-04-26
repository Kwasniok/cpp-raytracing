/**
 * @file
 * @brief utility module
 */

#ifndef CPP_RAYTRACING_UTIL_HPP
#define CPP_RAYTRACING_UTIL_HPP

#include <numbers>

namespace ray {

/**
 * clipping with compiletime boundaries
 */
template <typename T, T low, T high>
inline constexpr T clip(const T x) {
    if (x < low) {
        return low;
    }
    if (x > high) {
        return high;
    }
    return x;
}

/**
 * clipping with runtime boundaries
 */
template <typename T>
inline constexpr T clip(const T x, const T low, const T high) {
    if (x < low) {
        return low;
    }
    if (x > high) {
        return high;
    }
    return x;
}

} // namespace ray

#endif
