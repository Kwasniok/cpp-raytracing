#ifndef CPP_RAYTRACING_UTIL_H
#define CPP_RAYTRACING_UTIL_H

namespace ray {
template <typename T, T low, T high> inline T clip(const T x) {
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
