#include <cpp_raytracing/io/materials/dielectric.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Dielectric, const char*>> data = {
        {
            Dielectric{{0.0, 0.5, 1.0}, 0.7},
            "Dielectric {color = {0, 0.5, 1}, ior = 0.7}",
        },
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Dielectric, const char*>> data = {
        {
            Dielectric{{0.0, 0.5, 1.0}, 0.7},
            "Dielectric {color = {0, 0.5, 1}, ior = 0.7}",
        },
        // variations
        {
            Dielectric{{0.0, 0.5, 1.0}, 0.7},
            "Dielectric{color={0,0.5,1},ior=0.7}",
        },
        {
            Dielectric{{0.0, 0.5, 1.0}, 0.7},
            "Dielectric { color = { 0, 0.5, 1, } , ior = 0.7 }",
        },
    };
    for (const auto& [val, str] : data) {
        Dielectric x = io::read<Dielectric>(str);
        TEST_ASSERT_EQUAL(x.color, val.color);
        TEST_ASSERT_EQUAL(x.index_of_refraction, val.index_of_refraction);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        // variations
        "Dielectric {color = 0, 0.5, 1, ior=0.7}",
        "Dielectric {0, 0.5, 1}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Dielectric>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test