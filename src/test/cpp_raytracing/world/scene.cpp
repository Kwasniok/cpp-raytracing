#include "../../common.hpp"

#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>
#include <cpp_raytracing/world/scene.hpp>

#include "instance.hpp"
#include "sphere.hpp"

namespace cpp_raytracing { namespace test {

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
    using namespace tensor;

    auto dummy_camera = std::make_shared<PinholeCamera>(
        zero_vec<3_D>, [](const Scalar x, const Scalar y) {
            return Vec3{x, y, 0.0};
        });

    Scene scene(dummy_camera);
    scene.add(make_sphere(Vec3{1.0, 0.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 1.0, 0.0}, 0.5));
    scene.add(make_sphere(Vec3{0.0, 0.0, 1.0}, 0.5));
    return scene;
}

TEST_CASE("freeze") {

    const EuclideanGeometry geometry;
    Scene scene = make_scene();
    CHECK_FALSE(scene.is_frozen());

    {
        const Scene::FreezeGuard guard = scene.freeze_for_time(1.23);

        // frozen
        CHECK(scene.is_frozen());
        CHECK_THROWS_AS(scene.clear(), std::runtime_error);
        CHECK_THROWS_AS(scene.add(make_sphere(Vec3{}, 0.5)),
                        std::runtime_error);

        // hits
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{1.0, 0.0, 0.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 1.0, 0.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 0.0, 1.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK(record.hits());
        }
        // misses
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{-1.0, 0.0, 0.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK_FALSE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, -1.0, 0.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK_FALSE(record.hits());
        }
        {
            static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                                Vec3{0.0, 0.0, -1.0}};
            auto record = guard.hit_record(geometry, ray_segment, 0.0);
            CHECK_FALSE(record.hits());
        }
    }

    // unfrozen
    CHECK_FALSE(scene.is_frozen());
}

}} // namespace cpp_raytracing::test
