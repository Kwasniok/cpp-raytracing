#define BOOST_TEST_MODULE cpp_raytracing::world::scene

#include "../../common.hpp"

#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/cartesian_3d/euclidean.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>
#include <cpp_raytracing/world/scene.hpp>

#include "instance.hpp"
#include "sphere.hpp"

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

ray::Scene3D make_scene() {
    using ray::tensor::zero_vec;

    auto dummy_camera = std::make_shared<ray::PinholeCamera3D>(
        zero_vec<3_D>, [](const ray::Scalar x, const ray::Scalar y) {
            return ray::Vec3{x, y, 0.0};
        });

    ray::Scene3D scene(dummy_camera);
    scene.add(ray::make_sphere(ray::Vec3{1.0, 0.0, 0.0}, 0.5));
    scene.add(ray::make_sphere(ray::Vec3{0.0, 1.0, 0.0}, 0.5));
    scene.add(ray::make_sphere(ray::Vec3{0.0, 0.0, 1.0}, 0.5));
    return scene;
}

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct Scene3DFixture {
    Scene3DFixture() : scene{make_scene()} {}
    ~Scene3DFixture() = default;

    const ray::EuclideanCartesianGeometry3D geometry{};
    ray::Scene3D scene;
};

BOOST_FIXTURE_TEST_CASE(freeze, Scene3DFixture) {
    BOOST_CHECK(!scene.is_frozen());

    {
        const ray::Scene3D::FreezeGuard guard = scene.freeze_for_time(1.23);

        // frozen (= no modifications allowed)
        BOOST_CHECK(scene.is_frozen());
        BOOST_CHECK_THROW(scene.clear(), std::runtime_error);
        BOOST_CHECK_THROW(scene.add(ray::make_sphere(ray::Vec3{}, 0.5)),
                          std::runtime_error);
    } // unfrozen

    BOOST_CHECK(!scene.is_frozen());
}

BOOST_FIXTURE_TEST_CASE(hits, Scene3DFixture) {
    const ray::Scene3D::FreezeGuard guard = scene.freeze_for_time(1.23);

    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{1.0, 0.0, 0.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 1.0, 0.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 0.0, 1.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
}

BOOST_FIXTURE_TEST_CASE(misses, Scene3DFixture) {
    const ray::Scene3D::FreezeGuard guard = scene.freeze_for_time(1.23);
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{-1.0, 0.0, 0.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, -1.0, 0.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 0.0, -1.0}};
        auto record = guard.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
}
