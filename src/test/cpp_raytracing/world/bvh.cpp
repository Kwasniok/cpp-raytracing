#include <memory>
#include <vector>

#include <cpp_raytracing/world/bvh.hpp>
#include <cpp_raytracing/world/entities/instance.hpp>
#include <cpp_raytracing/world/entities/sphere.hpp>
#include <cpp_raytracing/world/ray.hpp>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-16;

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
            static constexpr Vec3 min{-0.5, -0.5, -0.5};
            static constexpr Vec3 max{1.5, 1.5, 1.5};
            const auto bounds = bvh_tree.bounding_box();
            TEST_ASSERT_TRUE(bounds.has_value());
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->min(), min, epsilon);
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(bounds->max(), max, epsilon);
        }

        // hits
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{1.0, 0.0, 0.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 1.0, 0.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, 1.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        // misses
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{-1.0, 0.0, 0.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, -1.0, 0.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, -1.0}};
            auto record = bvh_tree.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
    }
}

void run_test_suite() {
    run(test_all);
}

}} // namespace cpp_raytracing::test
