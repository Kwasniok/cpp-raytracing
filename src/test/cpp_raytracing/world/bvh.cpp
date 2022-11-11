#define BOOST_TEST_MODULE cpp_raytracing::world::bvh

#include "../../common.hpp"

#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/bvh.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>

#include "instance.hpp"
#include "sphere.hpp"

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1e-16;

struct BVHFixture {
    BVHFixture() {
        entities.push_back(ray::make_sphere(ray::Vec3{1.0, 0.0, 0.0}, 0.5));
        entities.push_back(ray::make_sphere(ray::Vec3{0.0, 1.0, 0.0}, 0.5));
        entities.push_back(ray::make_sphere(ray::Vec3{0.0, 0.0, 1.0}, 0.5));
    }
    ~BVHFixture() = default;

    const ray::EuclideanGeometry geometry{};
    std::vector<std::unique_ptr<ray::Entity>> entities{};
};

BOOST_FIXTURE_TEST_CASE(size, BVHFixture, *but::tolerance(epsilon)) {
    ray::BVHTree bvh_tree = ray::BVHTree(entities);

    BOOST_CHECK(bvh_tree.size_bounded() == entities.size());
    BOOST_CHECK(bvh_tree.size_unbounded() == 0ul);
    BOOST_CHECK(bvh_tree.size() == entities.size());
}

BOOST_FIXTURE_TEST_CASE(bounds, BVHFixture, *but::tolerance(epsilon)) {
    ray::BVHTree bvh_tree = ray::BVHTree(entities);

    constexpr ray::Vec3 min{-0.5, -0.5, -0.5};
    constexpr ray::Vec3 max{1.5, 1.5, 1.5};
    const auto bounds = bvh_tree.bounding_box();
    BOOST_CHECK(bounds.has_value());
    TEST_EQUAL_RANGES(bounds->min(), min);
    TEST_EQUAL_RANGES(bounds->max(), max);
}

BOOST_FIXTURE_TEST_CASE(hits, BVHFixture, *but::tolerance(epsilon)) {
    ray::BVHTree bvh_tree = ray::BVHTree(entities);

    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{1.0, 0.0, 0.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{0.0, 1.0, 0.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{0.0, 0.0, 1.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
}

BOOST_FIXTURE_TEST_CASE(misses, BVHFixture, *but::tolerance(epsilon)) {
    ray::BVHTree bvh_tree = ray::BVHTree(entities);

    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{-1.0, 0.0, 0.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{0.0, -1.0, 0.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                              ray::Vec3{0.0, 0.0, -1.0}};
        auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
}
