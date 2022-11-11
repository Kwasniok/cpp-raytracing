#define BOOST_TEST_MODULE cpp_raytracing::values::identifier

#include <omp.h>
#include <vector>

#include "../../common.hpp"
#include "../../common_omp.hpp"

#include <cpp_raytracing/values/random.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

const ray::Scalar epsilon = 1.e-12;

BOOST_AUTO_TEST_CASE(random_scalar_unit_scale, *but::tolerance(epsilon)) {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        const ray::Scalar res = ray::random_scalar();
        CHECK_IN_BOUNDS(res, 0.0, 1.0);
    }
}

BOOST_AUTO_TEST_CASE(random_scalar_arbitrary_scale, *but::tolerance(epsilon)) {
    constexpr ray::Scalar min = -7.11e+35;
    constexpr ray::Scalar max = 23.33e-27;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        const ray::Scalar res = ray::random_scalar(min, max);
        CHECK_IN_BOUNDS(res, min, max);
    }
}

BOOST_AUTO_TEST_CASE(random_scalar_multithreading, *but::tolerance(epsilon)) {
    constexpr int NUM_THREADS = 4;
    REQUIRE_OPENMP_MULTITHREADING<NUM_THREADS>();

    std::vector<ray::Scalar> results(static_cast<std::size_t>(NUM_THREADS));
#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < NUM_THREADS; ++i) {
        auto n = static_cast<std::size_t>(omp_get_thread_num());
        results[n] = ray::random_scalar(-2.0, 5.0);
    }
    // note: The check might have a (very small) chance of randomly causing
    //       a false-positive.
    CHECK_PAIRWISE_UNIQUE(results);
}
