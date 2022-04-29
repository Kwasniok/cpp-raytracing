/**
 * @file
 * @brief scalar type
 */

#ifndef CPP_RAYTRACING_SCALAR_HPP
#define CPP_RAYTRACING_SCALAR_HPP

#include <numbers>

namespace cpp_raytracing {

/** @brief scalar type */
using Scalar = double;

/** @brief positive infinity */
constexpr const Scalar infinity = std::numeric_limits<Scalar>::infinity();
/** @brief pi */
constexpr const Scalar pi = std::numbers::pi_v<Scalar>;

} // namespace cpp_raytracing

#endif
