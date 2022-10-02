#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/entities/bvh_collection.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>

#include <cpp_raytracing_test.hpp>

#include "../instance.hpp"
#include "../sphere.hpp"

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1e-16;

std::shared_ptr<Instance> make_sphere(const Vec3 position,
                                      const Scalar radius) {
    auto sphere = std::make_shared<Sphere>();
    sphere->radius = radius;

    auto instance = std::make_shared<Instance>();
    instance->entity = sphere;
    instance->position = position;

    return instance;
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
    static const Vec3 min{-0.5, -0.5, -0.5};
    static const Vec3 max{1.5, 1.5, 1.5};

    // cache
    TEST_ASSERT_THROWS(collection.bounding_box(), std::runtime_error);

    collection.generate_cache();
    const auto bounds = collection.bounding_box();
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->min(), min, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->max(), max, epsilon);
}

void test_hit_record() {

    const EuclideanGeometry geometry;
    BVHCollection collection = make_collection();

    // cache
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{1.0, 0.0, 0.0}};
        TEST_ASSERT_THROWS(collection.hit_record(geometry, ray_segment, 0.0),
                           std::runtime_error);
    }
    collection.generate_cache();

    // hits
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_TRUE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_TRUE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, 1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_TRUE(record.hits());
    }
    // misses
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{-1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_FALSE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, -1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_FALSE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, -1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        TEST_ASSERT_FALSE(record.hits());
    }
}

void run_test_suite() {
    run(test_bounding_box);
    run(test_hit_record);
}

}} // namespace cpp_raytracing::test
