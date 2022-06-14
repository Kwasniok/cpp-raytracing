
#include <utility>
#include <vector>

#include <cpp_raytracing/identifier.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

void test_default() {
    Identifier<void> identifier{};
    std::string expected = default_identifier<void>::value;
    expected += "_1";
    TEST_ASSERT_EQUAL(identifier.str(), expected);
}

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

void test_move() {
    {
        // move initialize
        Identifier<void> id1;
        const std::string str = id1.str();
        Identifier<void> id2 = std::move(id1);
        TEST_ASSERT_EQUAL(id2.str(), str);
    }
    {
        // move assign
        Identifier<void> id1;
        const std::string str = id1.str();
        Identifier<void> id2;
        id2 = std::move(id1);
        TEST_ASSERT_EQUAL(id2.str(), str);
    }
}

void test_change() {
    Identifier<void> id = Identifier<void>::make_always({"id"});
    id.change("test");
    TEST_ASSERT_EQUAL(id.str(), "test");
    {
        // change suffix for occupied id
        auto other_id = Identifier<void>::make_always({"occupided"});
        id.change("occupided");
        TEST_ASSERT_NOT_EQUAL(id.str(), "occupided");
        TEST_ASSERT_EQUAL(other_id.str(), "occupided");
    }
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
    TEST_ASSERT_EQUAL(id1.str(), "abc"); // precondition
    TEST_ASSERT_EQUAL(id2.str(), "ABC"); // precondition

    TEST_ASSERT_TRUE(id1.str() == id1.str());
    TEST_ASSERT_TRUE(id1.str() != id2.str());
}

void test_clone() {
    Identifier<void> id = Identifier<void>::make_always("abc");
    for (int l = 0; l < 2; ++l) {
        std::vector<Identifier<void>> clone_ids;
        for (int i = 2; i < 1005; ++i) {
            auto clone_id = id.clone();
            std::string expected = "abc";
            expected += '_';
            expected += std::to_string(i);
            TEST_ASSERT_EQUAL(clone_id.str(), expected);

            clone_ids.push_back(std::move(clone_id));
        }
    }
}

void run_test_suite() {
    run(test_default);
    run(test_make_if_available);
    run(test_make_always);
    run(test_move);
    run(test_change);
    run(test_valid_good);
    run(test_valid_bad);
    run(test_comparison);
    run(test_clone);
}

}} // namespace cpp_raytracing::test
