
#include <cpp_raytracing/io/materials/metal.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const auto val = Metal{"mat", {0.0, 0.5, 1.0}, 0.7};
    const auto str =
        "Metal {id = \"mat\", color = {0, 0.5, 1}, roughness = 0.7}";
    std::stringstream os;
    io::write(os, val);
    TEST_ASSERT_EQUAL(os.str(), str);
}

void test_read_success() {
    const auto val = Metal{"mat", {0.0, 0.5, 1.0}, 0.7};
    const std::vector<const char*> data = {
        "Metal {id = \"mat\", color = {0, 0.5, 1}, roughness = 0.7}",
        // variations
        "Metal{id=\"mat\",color={0,0.5,1},roughness=0.7}",
        "Metal { id = \"mat\" ,  color = { 0, 0.5, 1, } , roughness = 0.7 }",
    };
    for (const auto str : data) {
        Metal x = io::read<Metal>(str);
        TEST_ASSERT_EQUAL(x.color, val.color);
        TEST_ASSERT_EQUAL(x.roughness, val.roughness);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        // missing id
        "Metal {color = {0, 0.5, 1}, roughness = 0.7}",
        // missing braces
        "Metal {id = \"mat\", color = 0, 0.5, 1, roughness=0.7}",
        // missing fields
        "Metal {\"mat\", 0, 0.5, 1, 0.7}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Metal>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
