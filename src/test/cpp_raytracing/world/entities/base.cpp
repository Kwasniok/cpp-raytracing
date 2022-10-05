#include "../../../common.hpp"

#include <cpp_raytracing/world/entities/base.hpp>

namespace cpp_raytracing { namespace test {

class Dummy : public Entity {

    HitRecord hit_record(const Geometry&, const RaySegment&,
                         const Scalar) const override {
        return {.t = infinity};
    }
};

TEST_CASE("id") {
    Dummy dummy;
    dummy.id.change("test_id");
    CHECK(dummy.id.str() == "test_id");
}

}} // namespace cpp_raytracing::test
