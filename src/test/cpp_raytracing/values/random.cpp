#include "../../common.hpp"
#include "../../common_omp.hpp"

#include <cpp_raytracing/values/random.hpp>
#include <omp.h>

namespace cpp_raytracing { namespace test {

TEST_CASE("random_scalar_unit_scale") {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        CHECK_IN_RANGE(0.0, 1.0, random_scalar(0.0, 1.0));
    }
}

TEST_CASE("random_scalar_arbitrary_scale") {
    const Scalar min = -7.11e+35;
    const Scalar max = 23.33e-27;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        CHECK_IN_RANGE(min, max, random_scalar(min, max));
    }
}

TEST_CASE("random_scalar_multithreading") {
    constexpr int NUM_THREADS = 4;
    REQUIRE_OPENMP_MULTITHREADING<NUM_THREADS>();

    std::vector<Scalar> results(static_cast<std::size_t>(NUM_THREADS));
#pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < NUM_THREADS; ++i) {
        auto n = static_cast<std::size_t>(omp_get_thread_num());
        results[n] = random_scalar(-2.0, 5.0);
    }
    // note: The check might have a (very small) chance of randomly causing
    //       a false-positive.
    CHECK_PAIRWISE_UNIQUE(results);
}

}} // namespace cpp_raytracing::test
