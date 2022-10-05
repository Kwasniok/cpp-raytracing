#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <boost/range/adaptors.hpp>
#include <range/v3/all.hpp>

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_ITERABLE_APPROX_EQUAL(epsilon, range1, range2)                   \
    {                                                                          \
        const auto& r1 = range1;                                               \
        const auto& r2 = range2;                                               \
        for (auto [x, y] : ranges::views::zip(r1, r2)) {                       \
            CHECK(x == doctest::Approx(y).epsilon(epsilon));                   \
        }                                                                      \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_IN_RANGE(low, high, value)                                       \
    CHECK(low <= value);                                                       \
    CHECK(high >= value);

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_PAIRWISE_UNIQUE(range)                                           \
    for (const auto& [i, a] : range | boost::adaptors::indexed(0)) {           \
        for (const auto& [j, b] : range | boost::adaptors::indexed(0)) {       \
            if (i != j) {                                                      \
                CHECK(a != b);                                                 \
            }                                                                  \
        }                                                                      \
    }
