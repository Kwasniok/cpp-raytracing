#define BOOST_TEST_MODULE cpp_raytracing::values::identifier

#include <utility>
#include <vector>

#include "../../common.hpp"

#include <cpp_raytracing/values/identifier.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

BOOST_AUTO_TEST_CASE(default_constructor_with_default_identifier) {
    const ray::Identifier<void> identifier{};
    const std::string expected =
        std::string(ray::default_identifier<void>::value) + "_1";
    BOOST_CHECK(identifier.str() == expected);
}

BOOST_AUTO_TEST_CASE(make_if_available) {
    using Id = ray::Identifier<void>;
    const std::string str = "good_ID1";
    const auto opt_identifier = Id::make_if_available(str);
    BOOST_REQUIRE(opt_identifier);
    if (opt_identifier) {
        BOOST_CHECK(opt_identifier->str() == str);
    }
    BOOST_REQUIRE(!Id::make_if_available(str));
}

BOOST_AUTO_TEST_CASE(make_always) {
    using Id = ray::Identifier<void>;
    const std::string str = "good_ID1";
    const Id identifier = Id::make_always(str);
    BOOST_CHECK(identifier.str() == str);
}

BOOST_AUTO_TEST_CASE(move_construct) {
    using Id = ray::Identifier<void>;
    Id id1;
    const std::string str = id1.str();
    Id id2 = std::move(id1);
    BOOST_CHECK(id2.str() == str);
}

BOOST_AUTO_TEST_CASE(move_assignment) {
    using Id = ray::Identifier<void>;
    Id id1;
    Id id2;
    const std::string str = id1.str();
    id2 = std::move(id1);
    BOOST_CHECK(id2.str() == str);
}

BOOST_AUTO_TEST_CASE(change_to_free) {
    using Id = ray::Identifier<void>;
    Id id = Id::make_always({"id"});
    id.change("test");

    BOOST_CHECK(id.str() == "test");
}

BOOST_AUTO_TEST_CASE(change_to_occupied) {
    using Id = ray::Identifier<void>;
    Id id = Id::make_always({"id"});
    id.change("test");
    BOOST_REQUIRE(id.str() == "test");

    auto other_id = Id::make_always({"occupided"});
    id.change("occupided");
    BOOST_CHECK(id.str() != "occupided");
    BOOST_CHECK(other_id.str() == "occupided");
}

BOOST_AUTO_TEST_CASE(valid_good) {
    using Id = ray::Identifier<void>;
    const std::vector<std::string> values{
        "good_ID1", "1", "g", "G", "_",
    };
    for (const auto& v : values) {
        BOOST_CHECK(Id::valid(v));
    }
}

BOOST_AUTO_TEST_CASE(valid_bad) {
    using Id = ray::Identifier<void>;
    const std::vector<std::string> values{
        "",
        "bad ID",
        "bäd→ID",
    };
    for (const auto& v : values) {
        BOOST_CHECK(!Id::valid(v));
    }
}

BOOST_AUTO_TEST_CASE(comparison) {
    using Id = ray::Identifier<void>;
    Id id1 = Id::make_always("abc");
    Id id2 = Id::make_always("ABC");

    BOOST_REQUIRE(id1.str() == "abc");
    BOOST_REQUIRE(id2.str() == "ABC");

    BOOST_CHECK(id1.str() == id1.str());
    BOOST_CHECK(id1.str() != id2.str());
}

BOOST_AUTO_TEST_CASE(suffix_incremented_for_clones) {
    using Id = ray::Identifier<void>;
    Id id = Id::make_always("abc");
    constexpr int TEST_REPS = 2;
    for (int l = 0; l < TEST_REPS; ++l) {
        std::vector<Id> id_owner;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
        for (int i = 2; i < 1005; ++i) {
            auto clone_id = id.clone();
            std::string expected = "abc";
            expected += '_';
            expected += std::to_string(i);
            BOOST_CHECK(clone_id.str() == expected);

            id_owner.push_back(std::move(clone_id));
        }
    }
}
