#include "common.hpp"

namespace cpp_raytracing { namespace test {

TEST_CASE("example test case") {
    REQUIRE(true);
    SUBCASE("example subcase") {
        CHECK(true);
    }
}

}} // namespace cpp_raytracing::test