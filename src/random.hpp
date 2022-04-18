/**
 * @file
 * @brief random number generation
 */

#ifndef CPP_RAYTRACING_RANDOM_H
#define CPP_RAYTRACING_RANDOM_H

#include <random>

#include "scalar.hpp"

namespace ray {

/**
 * @brief generates a uniformly random number between the boundaries
 */
inline Scalar random_scalar(Scalar min, Scalar max) {
    return min + (max - min) * (Scalar(std::rand()) / RAND_MAX);
}

/**
 * @brief generates a uniformly random number between the boundaries
 */
template <Scalar min, Scalar max> inline Scalar random_scalar() {
    return min + (max - min) * (Scalar(std::rand()) / RAND_MAX);
}

} // namespace ray

#endif
