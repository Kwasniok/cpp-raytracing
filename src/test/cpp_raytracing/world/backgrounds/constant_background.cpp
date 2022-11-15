#define BOOST_TEST_MODULE cpp_raytracing::world::backgrounds::base

#include "../../../common.hpp"

#include <cpp_raytracing/world/backgrounds/constant_background.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

BOOST_AUTO_TEST_CASE(id) {
    ray::ConstantBackground3D background;
    background.id.change("test_id");
    BOOST_CHECK(background.id.str() == "test_id");
}
