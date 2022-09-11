#include <cpp_raytracing/world/materials/base.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

class Dummy : public Material {
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        return {ray, Color{}};
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
