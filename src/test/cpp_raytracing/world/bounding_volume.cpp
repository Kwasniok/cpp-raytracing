#include "../../common.hpp"

#include <limits>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/bounding_volume.hpp>

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1e-16;

TEST_CASE("constructor") {
    static const Vec3 min{1.0, 2.0, 3.0};
    static const Vec3 max{4.0, 5.0, 6.0};
    {
        static const AxisAlignedBoundingBox box{min, max};

        for (auto i = 0; i < 3; ++i) {
            CHECK(box.min()[i] == doctest::Approx(min[i]).epsilon(epsilon));
        }
        for (auto i = 0; i < 3; ++i) {
            CHECK(box.max()[i] == doctest::Approx(max[i]).epsilon(epsilon));
        }
    }
    {
        // max <-> min
        static const AxisAlignedBoundingBox box{max, min};

        for (auto i = 0; i < 3; ++i) {
            CHECK(box.min()[i] == doctest::Approx(min[i]).epsilon(epsilon));
        }
        for (auto i = 0; i < 3; ++i) {
            CHECK(box.max()[i] == doctest::Approx(max[i]).epsilon(epsilon));
        }
    }
}

TEST_CASE("hit") {
    static const Vec3 min{1.0, 2.0, 3.0};
    static const Vec3 max{4.0, 5.0, 6.0};
    static const Vec3 mid{2.5, 3.5, 4.5};
    static const AxisAlignedBoundingBox box{min, max};

    // start inside
    {
        static const RaySegment ray{mid, {7.0, 0.0, 0.0}};
        CHECK(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK(box.hit(ray, -100.0, 0.0));
        CHECK(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{mid, {0.0, 7.0, 0.0}};
        CHECK(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK(box.hit(ray, -100.0, 0.0));
        CHECK(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{mid, {0.0, 0.0, 7.0}};
        CHECK(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK(box.hit(ray, -100.0, 0.0));
        CHECK(box.hit(ray, -infinity, 0.0));
    }

    // start outside 'left'
    {
        static const RaySegment ray{Vec3{0.5, 3.5, 4.5}, {7.0, 0.0, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{Vec3{2.5, 1.5, 4.5}, {0.0, 7.0, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{Vec3{2.5, 3.5, 2.5}, {0.0, 0.0, 7.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }

    // start outside 'right'
    {
        static const RaySegment ray{Vec3{4.5, 3.5, 4.5}, {-7.0, 0.0, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{Vec3{2.5, 5.5, 4.5}, {0.0, -0.7, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static const RaySegment ray{Vec3{2.5, 3.5, 6.5}, {0.0, 0.0, -7.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 0.001));
        CHECK(box.hit(ray, 0.0, 100.0));
        CHECK(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
}

TEST_CASE("hit_corner_cases") {
    static const Vec3 min{1.0, 2.0, 3.0};
    static const Vec3 max{4.0, 5.0, 6.0};
    static const AxisAlignedBoundingBox box{min, max};

    {
        const RaySegment ray{Vec3{infinity, 3.5, 4.5}, {0.7, 0.0, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 100.0));
        CHECK_FALSE(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        const RaySegment ray{Vec3{2.5, infinity, 4.5}, {0.7, 0.0, 0.0}};
        CHECK_FALSE(box.hit(ray, 0.0, 100.0));
        CHECK_FALSE(box.hit(ray, 0.0, infinity));
        CHECK_FALSE(box.hit(ray, -100.0, 0.0));
        CHECK_FALSE(box.hit(ray, -infinity, 0.0));
    }
}

TEST_CASE("surrounding_box") {
    static const Vec3 min{1.0, 2.0, 3.0};
    static const Vec3 max{4.0, 5.0, 6.0};
    static const AxisAlignedBoundingBox box1{Vec3{1.0, 2.5, 5.0},
                                             Vec3{1.5, 3.0, 6.0}};
    static const AxisAlignedBoundingBox box2{Vec3{2.0, 2.0, 3.0},
                                             Vec3{4.0, 5.0, 5.0}};

    static const auto box = surrounding_box(box1, box2);

    CHECK_ITERABLE_APPROX_EQUAL(epsilon, box.min(), min);
    CHECK_ITERABLE_APPROX_EQUAL(epsilon, box.max(), max);
}

TEST_CASE("arithmetic") {
    static const AxisAlignedBoundingBox box{
        Vec3{1.0, 2.0, 3.0},
        Vec3{4.0, 5.0, 6.0},
    };
    static const Vec3 vec{7.0, 8.0, 9.0};

    // addition
    static const AxisAlignedBoundingBox expected{
        Vec3{8.0, 10.0, 12.0},
        Vec3{11.0, 13.0, 15.0},
    };
    {
        const AxisAlignedBoundingBox res = box + vec;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res.min(), expected.min());
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res.max(), expected.max());
    }
    {
        AxisAlignedBoundingBox res = box;
        res += vec;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res.min(), expected.min());
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res.max(), expected.max());
    }
}

}} // namespace cpp_raytracing::test
