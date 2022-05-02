#include <cpp_raytracing/io/materials/emitter.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Emitter, const char*>> data = {
        {Emitter{{0.0, 0.5, 1.0}}, "Emitter {color = {0, 0.5, 1}}"},
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Emitter, const char*>> data = {
        {Emitter{{0.0, 0.5, 1.0}}, "Emitter {color = {0, 0.5, 1}}"},
        // variations
        {Emitter{{0.0, 0.5, 1.0}}, "Emitter{color={0,0.5,1}}"},
        {Emitter{{0.0, 0.5, 1.0}}, "Emitter { color = { 0, 0.5, 1, } }"},
    };
    for (const auto& [val, str] : data) {
        Emitter x = io::read<Emitter>(str);
        TEST_ASSERT_EQUAL(x.color, val.color);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "Emitter {color = 0, 0.5, 1, }",
        "Emitter {0, 0.5, 1}",
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
