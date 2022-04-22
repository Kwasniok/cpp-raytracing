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
inline Scalar random_scalar(const Scalar min, const Scalar max) {
    // note: using constexpr does not work due to rand()
    // note: using template parameters is not supported by all compilers (yet)
    return min + (max - min) * (Scalar(std::rand()) / RAND_MAX);
}

} // namespace ray

#endif
