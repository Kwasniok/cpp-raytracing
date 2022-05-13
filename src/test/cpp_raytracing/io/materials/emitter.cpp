#include <cpp_raytracing/io/materials/emitter.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const auto val = Emitter{"mat", {0.0, 0.5, 1.0}};
    const auto str = "Emitter {id = \"mat\", color = {0, 0.5, 1}}";
    std::stringstream os;
    io::write(os, val);
    TEST_ASSERT_EQUAL(os.str(), str);
}

void test_read_success() {
    const auto val = Emitter{"mat", {0.0, 0.5, 1.0}};
    const std::vector<const char*> data = {
        "Emitter {id = \"mat\", color = {0, 0.5, 1}}",
        // variations
        "Emitter{id=\"mat\",color={0,0.5,1}}",
        "Emitter { id = \"mat\" ,  color = { 0, 0.5, 1, } }",
    };
    for (const auto str : data) {
        Emitter x = io::read<Emitter>(str);
        TEST_ASSERT_EQUAL(x.color, val.color);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        // missing id
        "Emitter {color = {0, 0.5, 1}}",
        // missing braces
        "Emitter {id = \"mat\", color = 0, 0.5, 1}",
        // missing fields
        "Emitter {\"mat\", 0, 0.5, 1}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Emitter>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
