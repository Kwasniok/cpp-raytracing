#include <cpp_raytracing/io/color.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Color, const char*>> data = {
        {Color{0.0, 0.5, 1.0}, "{0, 0.5, 1}"},
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Color, const char*>> data = {
        {Color{0.0, 0.5, 1.0}, "{0, 0.5, 1}"},
        // optional separator
        {Color{0.0, 0.5, 1.0}, "{0, 0.5, 1,}"},
        // spacing
        {Color{0.0, 0.5, 1.0}, "{0,0.5,1}"},
        {Color{0.0, 0.5, 1.0}, "{ 0,0.5,1 }"},
        {Color{0.0, 0.5, 1.0}, "{ 0, 0.5, 1 }"},
        // combinations
        {Color{0.0, 0.5, 1.0}, "{ 0 , 0.5 , 1 , }"},
    };
    for (const auto& [val, str] : data) {
        Color x = io::read<Color>(str);
        TEST_ASSERT_EQUAL(x, val);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        // no braces
        "0.0, 0.5, 1.0",
        // no separators
        "{0.0 0.5 1.0}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Color, io::grammar::color>(str)),
                           io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
