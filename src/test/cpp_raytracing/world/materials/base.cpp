#define BOOST_TEST_MODULE cpp_raytracing::world::materials::base

#include "../../../common.hpp"

#include <cpp_raytracing/world/materials/base.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

class Dummy : public ray::Material3D {

    std::pair<ray::Vec3, ray::Color>
    scatter([[maybe_unused]] const ray::HitRecord3D& record,
            const ray::Vec3& ray_direction) const override {
        return {ray_direction, ray::Colors::WHITE};
    }
};

BOOST_AUTO_TEST_CASE(identifier) {
    Dummy dummy;
    dummy.id.change("test_id");
    BOOST_CHECK(dummy.id.str() == "test_id");
}
