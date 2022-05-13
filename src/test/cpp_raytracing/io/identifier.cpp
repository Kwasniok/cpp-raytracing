#include <cpp_raytracing/io/identifier.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<const char*, const char*>> data = {
        {"good_ID1", "\"good_ID1\""},
        {"1", "\"1\""},
        {"g", "\"g\""},
        {"G", "\"G\""},
        {"_", "\"_\""},
    };
    for (const auto& [str, q_str] : data) {
        Identifier<void> val = Identifier<void>::make_always(str);
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), q_str);
    }
}

void test_read_success() {
    const std::vector<std::pair<const char*, const char*>> data = {
        {"good_ID1", "\"good_ID1\""},
        {"1", "\"1\""},
        {"g", "\"g\""},
        {"G", "\"G\""},
        {"_", "\"_\""},
    };
    for (const auto& [str, q_str] : data) {
        Identifier<void> x = io::read<Identifier<void>>(q_str);
        TEST_ASSERT_EQUAL(x, str);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "", "unquoted_ID", "\"\"", "\"bad ID\"", "\"bäd→ID\"",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Identifier<void>>(str)),
                           io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
