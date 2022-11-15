#define BOOST_TEST_MODULE cpp_raytracing::world::animators::triangle_animator

#include "../../../common.hpp"

#include <cpp_raytracing/world/animators/triangle_animator.hpp>
#include <cpp_raytracing/world/entities/triangle.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

class Dummy : public ray::TriangleAnimator3D {

    void update_for_time_hook(const ray::Scalar, ray::Triangle3D*) override {}
};

BOOST_AUTO_TEST_CASE(id) {
    Dummy anim;
    anim.id.change("test_id");
    BOOST_CHECK(anim.id.str() == "test_id");
}

BOOST_AUTO_TEST_CASE(update_for_time) {
    Dummy anim;
    anim.update_for_time(0, nullptr);
}
