#include "../../../common.hpp"

#include <memory>
#include <vector>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/world/entities/bvh_collection.hpp>
#include <cpp_raytracing/world/ray_segment.hpp>

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

TEST_CASE("bounding_box") {

    BVHCollection collection = make_collection();
    static const Vec3 min{-0.5, -0.5, -0.5};
    static const Vec3 max{1.5, 1.5, 1.5};

    // cache
    CHECK_THROWS_AS(collection.bounding_box(), std::runtime_error);

    collection.generate_cache();
    const auto bounds = collection.bounding_box();
    CHECK_ITERABLE_APPROX_EQUAL(epsilon, bounds->min(), min);
    CHECK_ITERABLE_APPROX_EQUAL(epsilon, bounds->max(), max);
}

TEST_CASE("hit_record") {

    const EuclideanGeometry geometry;
    BVHCollection collection = make_collection();

    // cache
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{1.0, 0.0, 0.0}};
        CHECK_THROWS_AS(collection.hit_record(geometry, ray_segment, 0.0),
                        std::runtime_error);
    }
    collection.generate_cache();

    // hits
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, 1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK(record.hits());
    }
    // misses
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{-1.0, 0.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK_FALSE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, -1.0, 0.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK_FALSE(record.hits());
    }
    {
        static const RaySegment ray_segment{Vec3{0.0, 0.0, 0.0},
                                            Vec3{0.0, 0.0, -1.0}};
        auto record = collection.hit_record(geometry, ray_segment, 0.0);
        CHECK_FALSE(record.hits());
    }
}

}} // namespace cpp_raytracing::test
