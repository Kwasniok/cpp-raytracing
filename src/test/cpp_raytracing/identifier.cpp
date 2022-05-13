
#include <utility>
#include <vector>

#include <cpp_raytracing/identifier.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

void test_make_if_available() {
    const char* str = "good_ID1";
    auto opt_identifier = Identifier<void>::make_if_available({str});
    TEST_ASSERT_TRUE(opt_identifier);
    if (opt_identifier) {
        TEST_ASSERT_EQUAL(opt_identifier->str(), str);
    }
}

void test_make_always() {
    const char* str{"good_ID1"};
    Identifier<void> identifier = Identifier<void>::make_always({str});
    TEST_ASSERT_EQUAL(identifier.str(), str);
}

void test_valid_good() {
    const std::vector<std::string> values{
        "good_ID1", "1", "g", "G", "_",
    };
    for (const auto& v : values) {
        TEST_ASSERT_TRUE(Identifier<void>::valid(v));
    }
}

void test_valid_bad() {
    const std::vector<std::string> values{
        "",
        "bad ID",
        "bäd→ID",
    };
    for (const auto& v : values) {
        TEST_ASSERT_FALSE(Identifier<void>::valid(v));
    }
}

void test_comparison() {
    Identifier<void> id1 = Identifier<void>::make_always("abc");
    Identifier<void> id2 = Identifier<void>::make_always("ABC");
    TEST_ASSERT_EQUAL(id1, "abc"); // precondition
    TEST_ASSERT_EQUAL(id2, "ABC"); // precondition

    TEST_ASSERT_TRUE(id1 == id1);
    TEST_ASSERT_TRUE(id1 != id2);
}

void run_test_suite() {
    run(test_make_if_available);
    run(test_make_always);
    run(test_valid_good);
    run(test_valid_bad);
    run(test_comparison);
}

}} // namespace cpp_raytracing::test
