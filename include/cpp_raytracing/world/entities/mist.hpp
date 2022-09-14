/**
 * @file
 * @brief hittable mist object
 */

#ifndef CPP_RAYTRACING_ENTITIES_MIST_HPP
#define CPP_RAYTRACING_ENTITIES_MIST_HPP

#include <cmath>

#include "../../values/random.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief mist object of constant density
 * @note Suitable for convex boundaries only!
 */
class Mist : public Entity {
  public:
    /** @brief boundary of the volume */
    std::shared_ptr<Entity> boundary;
    /** @brief material of the mist */
    std::shared_ptr<Material> material;
    /** @brief density of the medium */
    Scalar density = 1.0;

    virtual ~Mist() = default;

    virtual HitRecord hit_record(const Geometry& geometry,
                                 const RaySegment& ray,
                                 const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;
};

HitRecord Mist::hit_record(const Geometry& geometry, const RaySegment& ray,
                           const Scalar t_min, const Scalar t_max) const {

    if (!boundary) {
        // no boundary -> no effect
        return {.t = infinity};
    }

    HitRecord record1 =
        boundary->hit_record(geometry, ray, -infinity, infinity);
    if (record1.t == infinity) {
        return {.t = infinity};
    }

    const Scalar t_star = record1.t + std::abs(record1.t) * 1e-8;
    HitRecord record2 = boundary->hit_record(geometry, ray, t_star, infinity);
    if (record2.t == infinity) {
        return {.t = infinity};
    }

    if (record1.t < t_min) {
        record1.t = t_min;
    }
    if (record2.t > t_max) {
        record2.t = t_max;
    }

    if (record1.t >= record2.t) {
        return {.t = infinity};
    }

    if (record1.t < 0.0) {
        record1.t = 0.0;
    }

    const auto ray_length = ray.direction().length();
    const auto distance_inside_boundary = (record2.t - record1.t) * ray_length;
    const auto hit_distance =
        -1.0 / density * std::log(random_scalar(0.0, 1.0));

    if (hit_distance > distance_inside_boundary) {
        return {.t = infinity};
    }

    HitRecord record;
    record.t = record1.t + hit_distance / ray_length;
    record.point = ray.at(record.t);
    record.normal = Vec3{1.0, 0.0, 0.0}; // arbitrary
    record.front_face = true;            // arbitrary
    record.material = material.get();

    return record;
}

std::optional<AxisAlignedBoundingBox> Mist::bounding_box() const {
    if (boundary) {
        return boundary->bounding_box();
    };
    return std::nullopt;
}

} // namespace cpp_raytracing

#endif
