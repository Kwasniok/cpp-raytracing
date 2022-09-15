/**
 * @file
 * @brief hittable spherical object
 */

#ifndef CPP_RAYTRACING_TEST_ENTITIES_SPHERE_HPP
#define CPP_RAYTRACING_TEST_ENTITIES_SPHERE_HPP

#include <cmath>

#include <cpp_raytracing/world/entities/entity.hpp>

namespace cpp_raytracing { namespace test {

/**
 * @brief spherical object
 * @note Asserts Euclidean Geometry and ignores geometry parameter.
 */
class Sphere : public Entity {
  public:
    /**
     * @brief radius
     * @note Negative radii relate to spheres with the inside out. This is
     *       usefull for the inner surface of glass etc.
     */
    Scalar radius = 1.0;
    /** @brief material of the sphere */
    std::shared_ptr<Material> material;

    virtual ~Sphere() = default;

    virtual HitRecord hit_record(const Geometry& geometry,
                                 const RaySegment& ray,
                                 const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

  private:
    /**
     * @brief transforms normal vector of a sphere to uv coordinates
     * @note `u = phi / (2 * pi)`, `v = theta / pi`, y = -cos(theta)`, `x =
     *       -sin(theta) * cos(phi)` and `z = sin(theta) * sin(phi)`.
     * @note This uv mapping is similar but not identical to the canonical
     *       mappings in maths and physics.
     */
    static Vec2 uv_coordinates(const Vec3& normal) {
        const auto theta = std::acos(-normal.y());
        const auto phi = std::atan2(-normal.z(), normal.x()) + pi;

        return {phi / (2.0 * pi), theta / pi};
    }
};

HitRecord Sphere::hit_record(const Geometry& geometry, const RaySegment& ray,
                             const Scalar t_min, const Scalar t_max) const {

    // analytical geometry: line hits sphere
    // ray: s + t*d
    // sphere: (x-o)^2 = R^2
    // solve: a*t^2 + b*t + c = 0
    // where a = d^2 >= 0, b = 2*d*(s-o), c = (s-o)^2 - R^2
    // solution: t = (-b +/- sqrt(b^2 - 4ac))/(2a)
    const auto delta = ray.start();
    const auto a = dot(ray.direction(), ray.direction());
    const auto b_half = dot(ray.direction(), delta);
    const auto c = dot(delta, delta) - radius * radius;
    const auto discriminant = b_half * b_half - a * c;
    if (discriminant < 0.0) {
        // no real solution
        return HitRecord{.t = infinity};
    }

    // select minimal sloution in given range

    // t_+/- = (-b_half +/- sqrt(discriminant)) / a
    // t_->0, t_+>0: ray starts outside of the sphere and enters it at t_-
    // t_-<0, t_+>0: ray starts inside the sphere and leaves it at t_+
    // t_-<0, t_+<0: ray starts after the sphere and never enters it
    auto t = (-b_half - sqrt(discriminant)) / a;

    // select minimal positive sloution (if it exists)
    if (t < t_min || t > t_max) {
        t = (-b_half + sqrt(discriminant)) / a;
        if (t < t_min || t > t_max) {
            // no soltion in range
            return HitRecord{.t = infinity};
        }
    }

    // found solution in range
    const Vec3 point = ray.at(t);
    const Vec3 normal = point / radius;
    HitRecord record;
    record.t = t;
    record.point = point;
    record.set_face_normal(Mat3x3::identity(), Mat3x3::identity(),
                           ray.direction(), normal);
    record.uv_coordinates = uv_coordinates(normal);
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox> Sphere::bounding_box() const {
    return AxisAlignedBoundingBox{-Vec3{radius, radius, radius},
                                  Vec3{radius, radius, radius}};
}

}} // namespace cpp_raytracing::test

#endif
