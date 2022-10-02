#include <cpp_raytracing/values/random.hpp>
#include <omp.h>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

void test_random_scalar_unit_scale() {
    for (long i = 0; i < 1000; ++i) {
        TEST_ASSERT_IN_RANGE(random_scalar(0.0, 1.0), 0.0, 1.0);
    }
}

void test_random_scalar_arbitrary_scale() {
    const Scalar min = -7.11e+35;
    const Scalar max = 23.33e-27;
    for (long i = 0; i < 1000; ++i) {
        TEST_ASSERT_IN_RANGE(random_scalar(min, max), min, max);
    }
}

void test_random_scalar_multithreading() {
    TEST_ASSERT_OPENMP_IS_MULTITHREADING();
    const int N = std::min(4, omp_get_thread_limit());
    TEST_ASSERT_TRUE(N >= 4);

    std::vector<Scalar> results(static_cast<std::size_t>(N));
#pragma omp parallel for num_threads(N)
    for (int i = 0; i < N; ++i) {
        auto n = static_cast<std::size_t>(omp_get_thread_num());
        results[n] = random_scalar(-2.0, 5.0);
    }
    // note: The assertion might have a (very small) chance of randomly causing
    //       a false-positive.
    TEST_ASSERT_PAIRWISE_UNIQUE(results);
}

void run_test_suite() {
    run(test_random_scalar_unit_scale);
    run(test_random_scalar_arbitrary_scale);
    run(test_random_scalar_multithreading);
}

}} // namespace cpp_raytracing::test
