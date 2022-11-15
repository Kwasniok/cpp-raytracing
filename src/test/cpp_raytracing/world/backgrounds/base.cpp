#define BOOST_TEST_MODULE cpp_raytracing::world::backgrounds::base

#include "../../../common.hpp"

#include <cpp_raytracing/world/backgrounds/base.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

class Dummy : public ray::Background3D {

    ray::Color value(const ray::Geometry3D&,
                     const ray::RaySegment3D&) const override {
        return ray::Colors::RED;
    }
};

BOOST_AUTO_TEST_CASE(id) {
    Dummy dummy;
    dummy.id.change("test_id");
    BOOST_CHECK(dummy.id.str() == "test_id");
}
