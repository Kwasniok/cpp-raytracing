#include <cpp_raytracing/world/materials/base.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

class Dummy : public Material {
    virtual std::pair<Vec3, Color>
    scatter([[maybe_unused]] const HitRecord& record,
            const Vec3& ray_direction) const override {
        return {ray_direction, Colors::WHITE};
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
