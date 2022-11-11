#ifndef CPP_RAYTRACING_TEST_COMMON_HPP
#define CPP_RAYTRACING_TEST_COMMON_HPP

#include <boost/range/adaptors.hpp>
#include <range/v3/all.hpp>

#include <boost/test/included/unit_test.hpp>

/**
 * @brief tests for elementwise equaltiy (with tolerance)
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define TEST_EQUAL_RANGES(range1, range2)                                      \
    {                                                                          \
        const auto& r1 = range1;                                               \
        const auto& r2 = range2;                                               \
        auto it1 = std::begin(r1);                                             \
        auto it2 = std::begin(r2);                                             \
        const auto end1 = std::end(r1);                                        \
        const auto end2 = std::end(r2);                                        \
        std::size_t i = 0;                                                     \
        while (it1 != end1 && it2 != end2) {                                   \
            BOOST_TEST_INFO("Mismatch at position " << i << ": " << *it1       \
                                                    << " != " << *it2 << '.'); \
            BOOST_TEST(*it1 == *it2);                                          \
            ++i;                                                               \
            ++it1;                                                             \
            ++it2;                                                             \
        }                                                                      \
        if (it1 != end1) {                                                     \
            BOOST_FAIL(                                                        \
                "Mismatch in range size: Right range is shorter and has "      \
                << i << " elements.");                                         \
        }                                                                      \
        if (it2 != end2) {                                                     \
            BOOST_FAIL(                                                        \
                "Mismatch in range size: Left range is shorter and has "       \
                << i << " elements.");                                         \
        }                                                                      \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_IN_BOUNDS(value, min, max)                                       \
    BOOST_CHECK(value >= min);                                                 \
    BOOST_CHECK(value <= max);

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_PAIRWISE_UNIQUE(range)                                           \
    for (const auto& [i, a] : range | boost::adaptors::indexed(0)) {           \
        for (const auto& [j, b] : range | boost::adaptors::indexed(0)) {       \
            if (i != j) {                                                      \
                BOOST_CHECK(a != b);                                           \
            }                                                                  \
        }                                                                      \
    }

#endif
