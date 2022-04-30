/**
 * @file
 * @brief scalar type
 */

#ifndef CPP_RAYTRACING_SCALAR_HPP
#define CPP_RAYTRACING_SCALAR_HPP

#include <limits>
#include <numbers>

namespace cpp_raytracing {

/** @brief scalar type */
using Scalar = double;

/** @brief positive infinity */
constexpr const Scalar infinity = std::numeric_limits<Scalar>::infinity();
/** @brief pi */
constexpr const Scalar pi = std::numbers::pi_v<Scalar>;

/** @brief convert degrees to radiants */
constexpr inline Scalar rad_from_deg(const Scalar deg) {
    return deg * 2.0 * pi / 360.0;
}

/** @brief convert radiants to degrees */
constexpr inline Scalar deg_from_rad(const Scalar rad) {
    return rad * 360.0 / (2.0 * pi);
}

} // namespace cpp_raytracing

#endif
