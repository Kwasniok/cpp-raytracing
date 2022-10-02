#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/bvh.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>

#include <cpp_raytracing_test.hpp>

#include "instance.hpp"
#include "sphere.hpp"

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1e-16;

std::unique_ptr<Instance> make_sphere(const Vec3 position,
                                      const Scalar radius) {
    auto sphere = std::make_shared<Sphere>();
    sphere->radius = radius;

    auto instance = std::make_unique<Instance>();
    instance->entity = sphere;
    instance->position = position;

    return instance;
}

void test_all() {
    const EuclideanGeometry geometry;
    std::vector<std::unique_ptr<Entity>> entities;
    entities.push_back(make_sphere(Vec3{1.0, 0.0, 0.0}, 0.5));
    entities.push_back(make_sphere(Vec3{0.0, 1.0, 0.0}, 0.5));
    entities.push_back(make_sphere(Vec3{0.0, 0.0, 1.0}, 0.5));
    {
        const auto& container = entities; // access as const
        BVHTree bvh_tree = BVHTree(container);

        // size
        TEST_ASSERT_EQUAL(bvh_tree.size_bounded(), entities.size());
        TEST_ASSERT_EQUAL(bvh_tree.size_unbounded(), 0ul);
        TEST_ASSERT_EQUAL(bvh_tree.size(), entities.size());

        // bounds
        {
            static const Vec3 min{-0.5, -0.5, -0.5};
            static const Vec3 max{1.5, 1.5, 1.5};
            const auto bounds = bvh_tree.bounding_box();
            TEST_ASSERT_TRUE(bounds.has_value());
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->min(), min, epsilon);
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->max(), max, epsilon);
        }

        // hits
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{1.0, 0.0, 0.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 1.0, 0.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 0.0, 1.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_TRUE(record.hits());
        }
        // misses
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{-1.0, 0.0, 0.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, -1.0, 0.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 0.0, -1.0}};
            auto record = bvh_tree.hit_record(geometry, ray_segment, 0.0);
            TEST_ASSERT_FALSE(record.hits());
        }
    }
}

void run_test_suite() {
    run(test_all);
}

}} // namespace cpp_raytracing::test
