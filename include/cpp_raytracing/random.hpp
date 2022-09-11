/**
 * @file
 * @brief random number generation
 */

#ifndef CPP_RAYTRACING_RANDOM_HPP
#define CPP_RAYTRACING_RANDOM_HPP

#include <omp.h>
#include <random>

#include "values/scalar.hpp"

namespace cpp_raytracing {

/**
 * @brief generates a uniformly random number between the boundaries
 * @note thread-safe (each thread operates independently with a unique seed)
 */
inline Scalar random_scalar(const Scalar min, const Scalar max) {
    // IMPORTNANT: This version of the function with a thread_local static
    //             generator is the only one so far which did not cause a severe
    //             performance penalty!
    //             rand(), simply a static generator or using omp critical did
    //             all not work

    // note: using constexpr does not work due to rand()
    // note: using template parameters is not supported by all compilers (yet)
    // note: rand() is not guaranteed to be thread-safe
    // note: generator is expensive to create and must be preserved per thread
    //       and is therefore static
    static thread_local std::mt19937_64 generator(42 * omp_get_thread_num() +
                                                  1);
    // note: distribution is cheap to create and is therefore not static
    std::uniform_real_distribution<Scalar> distribution(min, max);
    return distribution(generator);
}

} // namespace cpp_raytracing

#endif
