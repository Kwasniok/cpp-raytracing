#include <cpp_raytracing/hittables/base.hpp>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

class Dummy : public Hittable {
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override {
        return {};
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
