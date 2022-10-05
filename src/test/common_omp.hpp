#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

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
        CHECK(buffer.at(i) == static_cast<int>(i));
    }
}
