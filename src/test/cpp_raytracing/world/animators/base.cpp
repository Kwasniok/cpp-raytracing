#define BOOST_TEST_MODULE cpp_raytracing::world::animators::base

#include "../../../common.hpp"

#include <cpp_raytracing/world/animators/base.hpp>
#include <cpp_raytracing/world/entities/base.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

class Dummy : public ray::Animator3D {

    void update_for_time(const ray::Scalar, ray::Entity3D*) override {}
};

BOOST_AUTO_TEST_CASE(id) {
    Dummy dummy;
    dummy.id.change("test_id");
    BOOST_CHECK(dummy.id.str() == "test_id");
}
