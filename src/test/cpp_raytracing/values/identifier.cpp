#include "../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/values/identifier.hpp>

namespace cpp_raytracing { namespace test {

TEST_CASE("default identifier") {
    const Identifier<void> identifier{};
    const std::string expected =
        std::string(default_identifier<void>::value) + "_1";
    CHECK(identifier.str() == expected);
}

TEST_CASE("make_if_available") {
    const std::string str = "good_ID1";
    const auto opt_identifier = Identifier<void>::make_if_available(str);
    REQUIRE(opt_identifier);
    if (opt_identifier) {
        CHECK(opt_identifier->str() == str);
    }
    REQUIRE_FALSE(Identifier<void>::make_if_available(str));
}

TEST_CASE("make_always") {
    const std::string str = "good_ID1";
    const Identifier<void> identifier = Identifier<void>::make_always(str);
    CHECK(identifier.str() == str);
}

TEST_CASE("move") {
    SUBCASE("move initialize") {
        Identifier<void> id1;
        const std::string str = id1.str();
        Identifier<void> id2 = std::move(id1);
        CHECK(id2.str() == str);
    }
    SUBCASE("move assignment") {
        Identifier<void> id1;
        const std::string str = id1.str();
        Identifier<void> id2;
        id2 = std::move(id1);
        CHECK(id2.str() == str);
    }
}

TEST_CASE("change") {
    Identifier<void> id = Identifier<void>::make_always({"id"});
    id.change("test");

    SUBCASE("free") {
        CHECK(id.str() == "test");
    }
    SUBCASE("occupied") {
        REQUIRE(id.str() == "test");
        auto other_id = Identifier<void>::make_always({"occupided"});
        id.change("occupided");
        CHECK(id.str() != "occupided");
        CHECK(other_id.str() == "occupided");
    }
}

TEST_CASE("valid") {
    SUBCASE("good") {
        const std::vector<std::string> values{
            "good_ID1", "1", "g", "G", "_",
        };
        for (const auto& v : values) {
            CHECK(Identifier<void>::valid(v));
        }
    }
    SUBCASE("bad") {
        const std::vector<std::string> values{
            "",
            "bad ID",
            "bäd→ID",
        };
        for (const auto& v : values) {
            CHECK_FALSE(Identifier<void>::valid(v));
        }
    }
}

TEST_CASE("comparison") {
    Identifier<void> id1 = Identifier<void>::make_always("abc");
    Identifier<void> id2 = Identifier<void>::make_always("ABC");
    REQUIRE(id1.str() == "abc");
    REQUIRE(id2.str() == "ABC");

    SUBCASE("equals") {
        CHECK(id1.str() == id1.str());
    }
    SUBCASE("unequals") {
        CHECK(id1.str() != id2.str());
    }
}

TEST_CASE("suffix incremented for clones") {
    Identifier<void> id = Identifier<void>::make_always("abc");
    constexpr int TEST_REPS = 2;
    for (int l = 0; l < TEST_REPS; ++l) {
        std::vector<Identifier<void>> id_owner;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        for (int i = 2; i < 1005; ++i) {
            auto clone_id = id.clone();
            std::string expected = "abc";
            expected += '_';
            expected += std::to_string(i);
            CHECK(clone_id.str() == expected);

            id_owner.push_back(std::move(clone_id));
        }
    }
}

}} // namespace cpp_raytracing::test
