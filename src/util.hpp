/**
 * @file
 * @brief utility module
 */

#ifndef CPP_RAYTRACING_UTIL_H
#define CPP_RAYTRACING_UTIL_H

#include <concepts>
#include <numbers>

namespace ray {

/**
 * clipping with compiletime boundaries
 */
template <typename T, T low, T high> inline constexpr T clip(const T x) {
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

/**
 * @brief integral division which rounds up
 * @note `ceil_idiv(n, d) == (n + d - 1) / d`
 */
template <std::integral T>
inline constexpr T ceil_idiv(const T numerator, const T denominator) {
    return (numerator + denominator - 1) / denominator;
}

} // namespace ray

#endif
