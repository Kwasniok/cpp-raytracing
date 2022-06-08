/**
 * @file
 * @brief utility module
 */

#ifndef CPP_RAYTRACING_UTIL_HPP
#define CPP_RAYTRACING_UTIL_HPP

#include <array>
#include <cstddef>
#include <iostream>
#include <numbers>
#include <tuple>
#include <utility>
#include <vector>

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

/** @brief write std::array to stream */
template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& array) {
    os << '{';
    bool first = true;
    for (auto& elem : array) {
        if (first) {
            first = false;
        } else {
            os << ", ";
        }
        os << elem;
    }
    os << '}';
    return os;
}

/** @brief write std::vector to stream */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& array) {
    os << '{';
    bool first = true;
    for (auto& elem : array) {
        if (first) {
            first = false;
        } else {
            os << ", ";
        }
        os << elem;
    }
    os << '}';
    return os;
}

/**
 * @brief iterate over a container with counter
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

/**
 * @brief iterate over a container and count the elements
 * @note complexity: O(n) = n
 */
template <typename T, typename TIter = decltype(std::begin(std::declval<T>()))>
constexpr std::size_t iterable_size(T&& iterable) {
    std::size_t size = 0;
    for (auto [index, elem] : enumerate<T, TIter>(iterable)) {
        size = index + 1;
    }
    return size;
}

/**
 * @brief returns pair with minimal value first and maximal value second
 *  @note like std::minmax but passes arguments by value
 */
template <typename T>
constexpr std::pair<T, T> minmax(const T x, const T y) {
    if (x < y) {
        return {x, y};
    }
    return {y, x};
}

} // namespace cpp_raytracing

#endif
