#define BOOST_TEST_MODULE cpp_raytracing::world::entities::base

#include "../../../common.hpp"

#include <memory>
#include <vector>

#include <cpp_raytracing/geometries/cartesian_3d/euclidean.hpp>
#include <cpp_raytracing/world/entities/bvh_collection.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>

#include "../instance.hpp"
#include "../sphere.hpp"

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1e-16;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct BVHCollection3DFixture {
    BVHCollection3DFixture() {
        collection.add(ray::make_sphere(ray::Vec3{1.0, 0.0, 0.0}, 0.5));
        collection.add(ray::make_sphere(ray::Vec3{0.0, 1.0, 0.0}, 0.5));
        collection.add(ray::make_sphere(ray::Vec3{0.0, 0.0, 1.0}, 0.5));
    }
    ~BVHCollection3DFixture() = default;

    const ray::cartesian_3d::EuclideanGeometry geometry{};
    ray::BVHCollection3D collection;
};

BOOST_FIXTURE_TEST_CASE(bounding_box_cache, BVHCollection3DFixture) {
    BOOST_CHECK_THROW(collection.bounding_box(), std::runtime_error);
    collection.generate_cache();
    collection.bounding_box();
}

BOOST_FIXTURE_TEST_CASE(bounding_box, BVHCollection3DFixture,
                        *but::tolerance(epsilon)) {
    collection.generate_cache();

    constexpr ray::Vec3 min{-0.5, -0.5, -0.5};
    constexpr ray::Vec3 max{1.5, 1.5, 1.5};

    const auto bounds = collection.bounding_box();
    TEST_EQUAL_RANGES(bounds->min(), min);
    TEST_EQUAL_RANGES(bounds->max(), max);
}

BOOST_FIXTURE_TEST_CASE(hit_record_cache, BVHCollection3DFixture) {
    constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                            ray::Vec3{1.0, 0.0, 0.0}};

    BOOST_CHECK_THROW(collection.hit_record(geometry, ray_segment, 0.0),
                      std::runtime_error);
    collection.generate_cache();
    collection.hit_record(geometry, ray_segment, 0.0);
}

BOOST_FIXTURE_TEST_CASE(hit_record_hits, BVHCollection3DFixture,
                        *but::tolerance(epsilon)) {
    collection.generate_cache();
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 0.0, 1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(record.hits());
    }
}

BOOST_FIXTURE_TEST_CASE(hit_record_misses, BVHCollection3DFixture,
                        *but::tolerance(epsilon)) {
    collection.generate_cache();
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{-1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, -1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
    {
        constexpr ray::RaySegment3D ray_segment{ray::Vec3{0.0, 0.0, 0.0},
                                                ray::Vec3{0.0, 0.0, -1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        BOOST_CHECK(!record.hits());
    }
}
