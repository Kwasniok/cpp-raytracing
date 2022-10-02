#include <cpp_raytracing/world/entities/base.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

class Dummy : public Entity {

    HitRecord hit_record(const Geometry&, const RaySegment&,
                         const Scalar) const override {
        return {.t = infinity};
    }
};

void test_id() {
    Dummy dummy;
    dummy.id.change("test_id");
    TEST_ASSERT_EQUAL(dummy.id.str(), "test_id");
}

void run_test_suite() {
    run(test_id);
}

}} // namespace cpp_raytracing::test
