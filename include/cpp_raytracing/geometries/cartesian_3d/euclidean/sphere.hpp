/**
 * @file
 * @brief hittable 3-dimensional Cartesian sphere
 */

#ifndef CPP_RAYTRACING_EGEOMETRIES_CARTESIAN_3D_CARTESIAN_SPHERE_HPP
#define CPP_RAYTRACING_EGEOMETRIES_CARTESIAN_3D_CARTESIAN_SPHERE_HPP

#include <algorithm>

#include "../../../world/entities/base.hpp"
#include "../../../world/materials/base.hpp"

namespace cpp_raytracing { namespace cartesian_3d {

/**
 * @brief 3-dimensional sphere (projected into first three dimensions)
 * @note Asserts n-dimensinal Euclidean geometry
 */
class Sphere3D : public Entity3D {
  public:
    /** @brief origin of the sphere */
    Vec3 position{};
    /**
     * @brief radius
     * @note Negative radii relate to spheres with the inside out. This is
     *       usefull for the inner surface of glass etc.
     */
    Scalar radius = 1.0;
    /** @brief material of the sphere */
    std::shared_ptr<Material3D> material;

    /** @brief default construct with default idenfifier root */
    Sphere3D() = default;
    /** @brief copy constructor */
    Sphere3D(const Sphere3D& other) = delete;
    /** @brief move constructor */
    Sphere3D(Sphere3D&& other) = default;
    /** @brief copy assignment */
    Sphere3D& operator=(const Sphere3D& other) = delete;
    /** @brief move assignment */
    Sphere3D& operator=(Sphere3D&& other) = default;

    ~Sphere3D() override = default;

    /** @see Entity::hit_record */
    HitRecord3D hit_record(const Geometry3D& geometry,
                           const RaySegment3D& ray_segment,
                           const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox3D> bounding_box() const override;
};

HitRecord3D Sphere3D::hit_record([[maybe_unused]] const Geometry3D& geometry,
                                 const RaySegment3D& ray_segment,
                                 const Scalar t_min) const {
    using namespace tensor;

    // analytical geometry: line hits sphere
    // ray: s + t*d
    // sphere: (x-o)^2 = R^2
    // solve: a*t^2 + b*t + c = 0
    // where a = d^2 >= 0, b = 2*d*(s-o), c = (s-o)^2 - R^2
    // solution: t = (-b +/- sqrt(b^2 - 4ac))/(2a)
    const auto delta = ray_segment.start() - position;
    const auto a = length_squared(ray_segment.direction());
    const auto b_half = dot(ray_segment.direction(), delta);
    const auto c = length_squared(delta) - radius * radius;
    const auto discriminant = b_half * b_half - a * c;
    if (discriminant < 0.0) {
        // no real solution
        return HitRecord3D{.t = infinity};
    }

    // select minimal sloution in given range

    // t_+/- = (-b_half +/- sqrt(discriminant)) / a
    // t_->0, t_+>0: ray starts outside of the sphere and enters it at t_-
    // t_-<0, t_+>0: ray starts inside the sphere and leaves it at t_+
    // t_-<0, t_+<0: ray starts after the sphere and never enters it
    auto t = (-b_half - sqrt(discriminant)) / a;

    // select minimal positive sloution (if it exists)
    if (t < t_min || t > ray_segment.t_max()) {
        t = (-b_half + sqrt(discriminant)) / a;
        if (t < t_min || t > ray_segment.t_max()) {
            // no soltion in range
            return HitRecord3D{.t = infinity};
        }
    }

    // found solution in range
    const Vec3 point = ray_segment.at(t);

    const Vec3 onb_normal = unit_vector((point - position));
    HitRecord3D record;
    record.t = t;
    record.point = point;
    record.set_local_geometry(ray_segment.direction(), onb_normal);
    record.uv_coordinates = {0, 0}; // dummy coordinates
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox3D> Sphere3D::bounding_box() const {
    const Vec3 corner{radius, radius, radius};
    return AxisAlignedBoundingBox3D{position - corner, position + corner};
}

}} // namespace cpp_raytracing::cartesian_3d

#endif
