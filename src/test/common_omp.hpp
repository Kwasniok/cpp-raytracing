#ifndef CPP_RAYTRACING_TEST_COMMON_OMP_HPP
#define CPP_RAYTRACING_TEST_COMMON_OMP_HPP

#include <boost/test/included/unit_test.hpp>

#include <array>
#include <omp.h>

template <std::size_t NUM_THREADS>
void REQUIRE_OPENMP_MULTITHREADING() {
    std::array<int, NUM_THREADS> buffer{};
#pragma omp parallel for num_threads(NUM_THREADS)
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        buffer.at(i) = omp_get_thread_num();
    }
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        if (buffer.at(i) != static_cast<int>(i)) {
            BOOST_FAIL("OMP not working as expected.");
        }
    }
}

#endif