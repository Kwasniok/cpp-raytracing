#include <cpp_raytracing/io/materials/diffuse.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Diffuse, const char*>> data = {
        {Diffuse{{0.0, 0.5, 1.0}}, "Diffuse {color = {0, 0.5, 1}}"},
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Diffuse, const char*>> data = {
        {Diffuse{{0.0, 0.5, 1.0}}, "Diffuse {color = {0, 0.5, 1}}"},
        // variations
        {Diffuse{{0.0, 0.5, 1.0}}, "Diffuse{color={0,0.5,1}}"},
        {Diffuse{{0.0, 0.5, 1.0}}, "Diffuse { color = { 0, 0.5, 1, } }"},
    };
    for (const auto& [val, str] : data) {
        Diffuse x = io::read<Diffuse>(str);
        TEST_ASSERT_EQUAL(x.color, val.color);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "Diffuse {color = 0, 0.5, 1, }",
        "Diffuse {0, 0.5, 1}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Diffuse>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
