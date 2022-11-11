#define BOOST_TEST_MODULE cpp_raytracing::world::bounding_volume

#include "../../common.hpp"

#include <limits>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/bounding_volume.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1e-16;

BOOST_AUTO_TEST_CASE(constructor, *but::tolerance(epsilon)) {
    constexpr ray::Vec3 min{1.0, 2.0, 3.0};
    constexpr ray::Vec3 max{4.0, 5.0, 6.0};
    {
        const ray::AxisAlignedBoundingBox box{min, max};

        TEST_EQUAL_RANGES(box.min(), min);
        TEST_EQUAL_RANGES(box.max(), max);
    }
    {
        // max <-> min
        const ray::AxisAlignedBoundingBox box{max, min};

        TEST_EQUAL_RANGES(box.min(), min);
        TEST_EQUAL_RANGES(box.max(), max);
    }
}

BOOST_AUTO_TEST_CASE(hit, *but::tolerance(epsilon)) {
    constexpr ray::Vec3 min{1.0, 2.0, 3.0};
    constexpr ray::Vec3 max{4.0, 5.0, 6.0};
    constexpr ray::Vec3 mid{2.5, 3.5, 4.5};
    const ray::AxisAlignedBoundingBox box{min, max};

    // start inside
    {
        constexpr ray::RaySegment ray{mid, {7.0, 0.0, 0.0}};
        BOOST_CHECK(box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{mid, {0.0, 7.0, 0.0}};
        BOOST_CHECK(box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{mid, {0.0, 0.0, 7.0}};
        BOOST_CHECK(box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(box.hit(ray, -ray::infinity, 0.0));
    }

    // start outside 'left'
    {
        constexpr ray::RaySegment ray{ray::Vec3{0.5, 3.5, 4.5},
                                      {7.0, 0.0, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{ray::Vec3{2.5, 1.5, 4.5},
                                      {0.0, 7.0, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{ray::Vec3{2.5, 3.5, 2.5},
                                      {0.0, 0.0, 7.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }

    // start outside 'right'
    {
        constexpr ray::RaySegment ray{ray::Vec3{4.5, 3.5, 4.5},
                                      {-7.0, 0.0, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{ray::Vec3{2.5, 5.5, 4.5},
                                      {0.0, -0.7, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
    {
        constexpr ray::RaySegment ray{ray::Vec3{2.5, 3.5, 6.5},
                                      {0.0, 0.0, -7.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 0.001));
        BOOST_CHECK(box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
}

BOOST_AUTO_TEST_CASE(hit_corner_cases, *but::tolerance(epsilon)) {
    constexpr ray::Vec3 min{1.0, 2.0, 3.0};
    constexpr ray::Vec3 max{4.0, 5.0, 6.0};
    const ray::AxisAlignedBoundingBox box{min, max};

    {
        const ray::RaySegment ray{ray::Vec3{ray::infinity, 3.5, 4.5},
                                  {0.7, 0.0, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(!box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
    {
        const ray::RaySegment ray{ray::Vec3{2.5, ray::infinity, 4.5},
                                  {0.7, 0.0, 0.0}};
        BOOST_CHECK(!box.hit(ray, 0.0, 100.0));
        BOOST_CHECK(!box.hit(ray, 0.0, ray::infinity));
        BOOST_CHECK(!box.hit(ray, -100.0, 0.0));
        BOOST_CHECK(!box.hit(ray, -ray::infinity, 0.0));
    }
}

BOOST_AUTO_TEST_CASE(surrounding_box, *but::tolerance(epsilon)) {
    constexpr ray::Vec3 min{1.0, 2.0, 3.0};
    constexpr ray::Vec3 max{4.0, 5.0, 6.0};
    const ray::AxisAlignedBoundingBox box1{ray::Vec3{1.0, 2.5, 5.0},
                                           ray::Vec3{1.5, 3.0, 6.0}};
    const ray::AxisAlignedBoundingBox box2{ray::Vec3{2.0, 2.0, 3.0},
                                           ray::Vec3{4.0, 5.0, 5.0}};

    const auto box = ray::surrounding_box(box1, box2);

    TEST_EQUAL_RANGES(box.min(), min);
    TEST_EQUAL_RANGES(box.max(), max);
}

BOOST_AUTO_TEST_CASE(arithmetic, *but::tolerance(epsilon)) {
    const ray::AxisAlignedBoundingBox box{
        ray::Vec3{1.0, 2.0, 3.0},
        ray::Vec3{4.0, 5.0, 6.0},
    };
    constexpr ray::Vec3 vec{7.0, 8.0, 9.0};

    // addition
    const ray::AxisAlignedBoundingBox expected{
        ray::Vec3{8.0, 10.0, 12.0},
        ray::Vec3{11.0, 13.0, 15.0},
    };
    {
        const ray::AxisAlignedBoundingBox res = box + vec;
        TEST_EQUAL_RANGES(res.min(), expected.min());
        TEST_EQUAL_RANGES(res.max(), expected.max());
    }
    {
        ray::AxisAlignedBoundingBox res = box;
        res += vec;
        TEST_EQUAL_RANGES(res.min(), expected.min());
        TEST_EQUAL_RANGES(res.max(), expected.max());
    }
}
