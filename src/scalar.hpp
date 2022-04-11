#ifndef CPP_RAYTRACING_SCALAR_H
#define CPP_RAYTRACING_SCALAR_H

#include <numbers>

namespace {
using Scalar = double;

constexpr const Scalar SCALAR_INF = std::numeric_limits<Scalar>::infinity();
constexpr const Scalar SCALAR_PI = std::numbers::pi_v<Scalar>;

} // namespace

#endif
