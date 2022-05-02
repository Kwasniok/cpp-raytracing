#include <cpp_raytracing/io/scalar.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Scalar, const char*>> data = {
        // negative
        {-1.234, "-1.234"},
        // positive
        {+1.234, "1.234"},
        // precision
        {1.23456789012345678901234567890, "1.2345678901234567"},
        // exponential representaion
        {1.23456e+99, "1.23456e+99"},
        // combinations
        {-1.23456e-99, "-1.23456e-99"},
        // inf
        {infinity, "inf"},
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Scalar, const char*>> data = {
        // negative
        {-1.234, "-1.234"},
        // positive
        {+1.234, "1.234"},
        // precision
        {1.23456789012345678901234567890, "1.2345678901234567"},
        // exponential representaion
        {1.23456e+99, "1.23456e+99"},
        // combinations
        {-1.23456e-99, "-1.23456e-99"},
        // inf
        {infinity, "inf"},
        {infinity, "+inf"},
        {-infinity, "-inf"},
    };
    for (const auto& [val, str] : data) {
        Scalar x = io::read<Scalar>(str);
        TEST_ASSERT_EQUAL(x, val);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        // nan
        "nan",
        "NaN",
        "NAN",
        // short hands
        "0.",
        ".0",
        "+.0",
        "-.0",
        "e12",
        // capitals
        "1E9",
        "INF",
        "+INF",
        "-INF",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Scalar>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
