#define BOOST_TEST_MODULE cpp_raytracing::world::entities::base

#include "../../../common.hpp"

#include <cpp_raytracing/world/entities/base.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

class Dummy : public ray::Entity {

    ray::HitRecord hit_record(const ray::Geometry&, const ray::RaySegment&,
                              const ray::Scalar) const override {
        return {.t = ray::infinity};
    }
};

BOOST_AUTO_TEST_CASE(id) {
    Dummy dummy;
    dummy.id.change("test_id");
    BOOST_CHECK(dummy.id.str() == "test_id");
}
