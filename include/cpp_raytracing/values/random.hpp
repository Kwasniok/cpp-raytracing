/**
 * @file
 * @brief random number generation
 */

#ifndef CPP_RAYTRACING_RANDOM_HPP
#define CPP_RAYTRACING_RANDOM_HPP

#include <omp.h>
#include <random>

#include "scalar.hpp"

namespace cpp_raytracing {

/** @brief returns unique number for each thread */
inline std::mt19937_64::result_type get_thread_seed() {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    return static_cast<std::mt19937_64::result_type>(42 * omp_get_thread_num() +
                                                     1);
}

/**
 * @brief generates a uniformly random number between the boundaries
 * @note thread-safe (each thread operates independently with a unique seed)
 */
inline Scalar random_scalar(const Scalar min = 0.0, const Scalar max = 1.0) {
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
    static thread_local std::mt19937_64 generator(get_thread_seed());
    // note: distribution is cheap to create and is therefore not static
    std::uniform_real_distribution<Scalar> distribution(min, max);
    return distribution(generator);
}

} // namespace cpp_raytracing

#endif
