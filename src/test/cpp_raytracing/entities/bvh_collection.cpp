#include <memory>
#include <vector>

#include <cpp_raytracing/entities/bvh_collection.hpp>
#include <cpp_raytracing/entities/sphere.hpp>
#include <cpp_raytracing/ray.hpp>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-16;

std::unique_ptr<Sphere> make_sphere(const Vec3 position, const Scalar radius) {
    auto sphere = std::make_unique_for_overwrite<Sphere>();
    sphere->position = position;
    sphere->radius = radius;
    return sphere;
}

BVHCollection make_collection() {
    BVHCollection collection;
    collection.add(make_sphere(Vec3{1.0, 0.0, 0.0}, 0.5));
    collection.add(make_sphere(Vec3{0.0, 1.0, 0.0}, 0.5));
    collection.add(make_sphere(Vec3{0.0, 0.0, 1.0}, 0.5));
    return collection;
}

void test_bounding_box() {

    BVHCollection collection = make_collection();
    static constexpr Vec3 min{-0.5, -0.5, -0.5};
    static constexpr Vec3 max{1.5, 1.5, 1.5};

    // cache
    TEST_ASSERT_THROWS(collection.bounding_box(), std::runtime_error);

    collection.generate_cache();
    const auto bounds = collection.bounding_box();
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->min(), min, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->max(), max, epsilon);
}

void test_hit_record() {

    BVHCollection collection = make_collection();

    // cache
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{1.0, 0.0, 0.0}};
        TEST_ASSERT_THROWS(collection.hit_record(ray, 0.0, infinity),
                           std::runtime_error);
    }
    collection.generate_cache();

    // hits
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{1.0, 0.0, 0.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_TRUE(record.hits());
    }
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 1.0, 0.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_TRUE(record.hits());
    }
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, 1.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_TRUE(record.hits());
    }
    // misses
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{-1.0, 0.0, 0.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_FALSE(record.hits());
    }
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, -1.0, 0.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_FALSE(record.hits());
    }
    {
        static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, -1.0}};
        auto record = collection.hit_record(ray, 0.0, infinity);
        TEST_ASSERT_FALSE(record.hits());
    }
}

void run_test_suite() {
    run(test_bounding_box);
    run(test_hit_record);
}

}} // namespace cpp_raytracing::test
