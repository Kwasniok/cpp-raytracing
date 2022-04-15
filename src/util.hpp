#ifndef CPP_RAYTRACING_UTIL_H
#define CPP_RAYTRACING_UTIL_H

#include <numbers>

namespace ray {

template <typename T> inline T degrees_to_radians(T degrees) {
    return degrees * std::numbers::pi_v<T> / 180.0;
}

template <typename T, T low, T high> inline constexpr T clip(const T x) {
    if (x < low) {
        return low;
    }
    if (x > high) {
        return high;
    }
    return x;
}

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
