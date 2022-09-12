#include <limits>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/bounding_volume.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-16;

void test_constructor() {
    static constexpr Vec3 min{1.0, 2.0, 3.0};
    static constexpr Vec3 max{4.0, 5.0, 6.0};
    {
        static constexpr AxisAlignedBoundingBox box{min, max};

        TEST_ASSERT_EQUAL(box.min(), min);
        TEST_ASSERT_EQUAL(box.max(), max);
    }
    {
        // max <-> min
        static constexpr AxisAlignedBoundingBox box{max, min};

        TEST_ASSERT_EQUAL(box.min(), min);
        TEST_ASSERT_EQUAL(box.max(), max);
    }
}

void test_hit() {
    static constexpr Vec3 min{1.0, 2.0, 3.0};
    static constexpr Vec3 max{4.0, 5.0, 6.0};
    static constexpr Vec3 mid{2.5, 3.5, 4.5};
    static constexpr AxisAlignedBoundingBox box{min, max};

    // start inside
    {
        static constexpr RaySegment ray{mid, {7.0, 0.0, 0.0}};
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_TRUE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_TRUE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{mid, {0.0, 7.0, 0.0}};
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_TRUE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_TRUE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{mid, {0.0, 0.0, 7.0}};
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_TRUE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_TRUE(box.hit(ray, -infinity, 0.0));
    }

    // start outside 'left'
    {
        static constexpr RaySegment ray{Vec3{0.5, 3.5, 4.5}, {7.0, 0.0, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{Vec3{2.5, 1.5, 4.5}, {0.0, 7.0, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{Vec3{2.5, 3.5, 2.5}, {0.0, 0.0, 7.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }

    // start outside 'right'
    {
        static constexpr RaySegment ray{Vec3{4.5, 3.5, 4.5}, {-7.0, 0.0, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{Vec3{2.5, 5.5, 4.5}, {0.0, -0.7, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        static constexpr RaySegment ray{Vec3{2.5, 3.5, 6.5}, {0.0, 0.0, -7.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 0.001));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_TRUE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
}

void test_hit_corner_cases() {
    static constexpr Vec3 min{1.0, 2.0, 3.0};
    static constexpr Vec3 max{4.0, 5.0, 6.0};
    static constexpr Vec3 mid{2.5, 3.5, 4.5};
    static constexpr AxisAlignedBoundingBox box{min, max};

    {
        const RaySegment ray{Vec3{infinity, 3.5, 4.5}, {0.7, 0.0, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
    {
        const RaySegment ray{Vec3{2.5, infinity, 4.5}, {0.7, 0.0, 0.0}};
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, 100.0));
        TEST_ASSERT_FALSE(box.hit(ray, 0.0, infinity));
        TEST_ASSERT_FALSE(box.hit(ray, -100.0, 0.0));
        TEST_ASSERT_FALSE(box.hit(ray, -infinity, 0.0));
    }
}

void test_surrounding_box() {
    static constexpr Vec3 min{1.0, 2.0, 3.0};
    static constexpr Vec3 max{4.0, 5.0, 6.0};
    static constexpr AxisAlignedBoundingBox box1{Vec3{1.0, 2.5, 5.0},
                                                 Vec3{1.5, 3.0, 6.0}};
    static constexpr AxisAlignedBoundingBox box2{Vec3{2.0, 2.0, 3.0},
                                                 Vec3{4.0, 5.0, 5.0}};

    static constexpr auto box = surrounding_box(box1, box2);

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(box.min(), min, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(box.max(), max, epsilon);
}

void test_arithmetic() {
    static constexpr AxisAlignedBoundingBox box{
        Vec3{1.0, 2.0, 3.0},
        Vec3{4.0, 5.0, 6.0},
    };
    static constexpr Vec3 vec{7.0, 8.0, 9.0};

    // addition
    static constexpr AxisAlignedBoundingBox expected{
        Vec3{8.0, 10.0, 12.0},
        Vec3{11.0, 13.0, 15.0},
    };
    {
        const AxisAlignedBoundingBox res = box + vec;
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(res.min(), expected.min(), epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(res.max(), expected.max(), epsilon);
    }
    {
        AxisAlignedBoundingBox res = box;
        res += vec;
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(res.min(), expected.min(), epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(res.max(), expected.max(), epsilon);
    }
}

void run_test_suite() {
    run(test_constructor);
    run(test_hit);
    run(test_hit_corner_cases);
    run(test_arithmetic);
}

}} // namespace cpp_raytracing::test
