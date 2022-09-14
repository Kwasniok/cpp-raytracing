#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/entities/instance.hpp>
#include <cpp_raytracing/world/entities/sphere.hpp>
#include <cpp_raytracing/world/ray.hpp>
#include <cpp_raytracing/world/scene.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-16;

std::shared_ptr<Instance> make_sphere(const Vec3 position,
                                      const Scalar radius) {
    auto sphere = std::make_shared<Sphere>();
    sphere->radius = radius;

    auto instance = std::make_shared<Instance>();
    instance->entity = sphere;
    instance->position = position;

    return instance;
}

Scene make_scene() {
    Scene scene;
    scene.add(make_sphere(Vec3{1.0, 0.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 1.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 0.0, 1.0}, 0.5));
    return scene;
}

void test_freeze() {

    const EuclideanGeometry geometry;
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
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{1.0, 0.0, 0.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 1.0, 0.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        {
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, 1.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
            TEST_ASSERT_TRUE(record.hits());
        }
        // misses
        {
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{-1.0, 0.0, 0.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, -1.0, 0.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
            TEST_ASSERT_FALSE(record.hits());
        }
        {
            static constexpr RaySegment ray{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, -1.0}};
            auto record = guard.hit_record(geometry, ray, 0.0, infinity);
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
