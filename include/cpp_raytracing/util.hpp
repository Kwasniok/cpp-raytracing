/**
 * @file
 * @brief utility module
 */

#ifndef CPP_RAYTRACING_UTIL_HPP
#define CPP_RAYTRACING_UTIL_HPP

#include <numbers>
#include <tuple>

namespace cpp_raytracing {

/**
 * @brief clipping with compiletime boundaries
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
 * @brief clipping with runtime boundaries
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
 * @brief iterate over a container wiht counter
 * @note example: @code
 * std::array<int, 3> list= {1, 2, 3};
 * for (auto [i, elem] : enumerate(list))
 * {
 *     // i is the index and elem is the i-th element
 * }
 * @endcode
 */
template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T&& iterable) {
    struct iterator {
        size_t i;
        TIter iter;
        bool operator!=(const iterator& other) const {
            return iter != other.iter;
        }
        void operator++() {
            ++i;
            ++iter;
        }
        auto operator*() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper {
        T iterable;
        auto begin() { return iterator{0, std::begin(iterable)}; }
        auto end() { return iterator{0, std::end(iterable)}; }
    };
    return iterable_wrapper{std::forward<T>(iterable)};
}

} // namespace cpp_raytracing

#endif
