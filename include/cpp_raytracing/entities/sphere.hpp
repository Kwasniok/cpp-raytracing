/**
 * @file
 * @brief hittable spherical object
 */

#ifndef CPP_RAYTRACING_ENTITIES_SHPERE_HPP
#define CPP_RAYTRACING_ENTITIES_SHPERE_HPP

#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief spherical object
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

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;
};

HitRecord Sphere::hit_record(const Ray& ray, const Scalar t_min,
                             const Scalar t_max) const {

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
    record.set_face_normal(ray, normal);
    record.material = material;
    return record;
}

std::optional<AxisAlignedBoundingBox> Sphere::bounding_box() const {
    return AxisAlignedBoundingBox{-Vec3{radius, radius, radius},
                                  Vec3{radius, radius, radius}};
}

} // namespace cpp_raytracing

#endif
