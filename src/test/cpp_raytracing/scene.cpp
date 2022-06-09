#include <memory>
#include <vector>

#include <cpp_raytracing/entities/sphere.hpp>
#include <cpp_raytracing/ray.hpp>
#include <cpp_raytracing/scene.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-16;

std::unique_ptr<Sphere> make_sphere(const Vec3 position, const Scalar radius) {
    auto sphere = std::make_unique_for_overwrite<Sphere>();
    sphere->position = position;
    sphere->radius = radius;
    return sphere;
}

Scene make_scene() {
    Scene scene;
    scene.add(make_sphere(Vec3{1.0, 0.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 1.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 0.0, 1.0}, 0.5));
    return scene;
}

void test_freeze() {

    Scene scene = make_scene();
    TEST_ASSERT_FALSE(scene.is_frozen());

    {
        const Scene::FreezeGuard guard = scene.freeze_for_time(1.23);

        // frozen
        TEST_ASSERT_TRUE(scene.is_frozen());
        TEST_ASSERT_THROWS(scene.clear(), std::runtime_error);
        TEST_ASSERT_THROWS(scene.add(make_sphere(Vec3{}, 0.5)),
                           std::runtime_error);

        // hits
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{1.0, 0.0, 0.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 1.0, 0.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, 1.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        // misses
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{-1.0, 0.0, 0.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, -1.0, 0.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr Ray ray{Vec3{0.0, 0.0, 0.0}, Vec3{0.0, 0.0, -1.0}};
            auto record = guard.hit_record(ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
    }

    // unfrozen
    TEST_ASSERT_FALSE(scene.is_frozen());
}

void run_test_suite() {
    run(test_freeze);
}

}} // namespace cpp_raytracing::test