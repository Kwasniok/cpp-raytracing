/**
 * @file
 * @brief hittable n-dimensional Cartesian sphere
 */

#ifndef CPP_RAYTRACING_EGEOMETRIES_CARTESIAN_EMBEDDED_SPHERE_HPP
#define CPP_RAYTRACING_EGEOMETRIES_CARTESIAN_EMBEDDED_SPHERE_HPP

#include <algorithm>

#include "../../world/entities/mesh.hpp"
#include "../../world/entities/small_triangle_util.hpp"

namespace cpp_raytracing { namespace cartesian_embedded {

/**
 * @brief n-dimensional sphere (projected into first three dimensions)
 * @note Asserts n-dimensinal Euclidean geometry
 */
template <Dimension DIMENSION>
requires(DIMENSION >= 3) class Sphere : public Entity<DIMENSION> {
  public:
    /** @brief origin of the sphere */
    Vec<DIMENSION> position{};
    /**
     * @brief radius
     * @note Negative radii relate to spheres with the inside out. This is
     *       usefull for the inner surface of glass etc.
     */
    Scalar radius = 1.0;
    /** @brief material of the sphere */
    std::shared_ptr<Material<DIMENSION>> material;

    /** @brief default construct with default idenfifier root */
    Sphere() = default;
    /** @brief copy constructor */
    Sphere(const Sphere& other) = delete;
    /** @brief move constructor */
    Sphere(Sphere&& other) = default;
    /** @brief copy assignment */
    Sphere& operator=(const Sphere& other) = delete;
    /** @brief move assignment */
    Sphere& operator=(Sphere&& other) = default;

    ~Sphere() override = default;

    /** @see Entity::hit_record */
    HitRecord<DIMENSION> hit_record(const Geometry<DIMENSION>& geometry,
                                    const RaySegment<DIMENSION>& ray_segment,
                                    const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override;
};

template <Dimension DIMENSION>
requires(DIMENSION >= 3) HitRecord<DIMENSION> Sphere<DIMENSION>::hit_record(
    const Geometry<DIMENSION>& geometry,
    const RaySegment<DIMENSION>& ray_segment, const Scalar t_min)
const {
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
        return HitRecord<DIMENSION>{.t = infinity};
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
            return HitRecord<DIMENSION>{.t = infinity};
        }
    }

    // found solution in range
    const Vec<DIMENSION> point = ray_segment.at(t);
    const Mat<DIMENSION> metric = geometry.metric(point);
    const Mat<3_D, DIMENSION> to_onb_jacobian = geometry.to_onb_jacobian(point);

    const Vec3 onb_normal = unit_vector(to_onb_jacobian * (point - position));
    HitRecord<DIMENSION> record;
    record.t = t;
    record.point = point;
    record.set_local_geometry(to_onb_jacobian * ray_segment.direction(),
                              onb_normal);
    record.uv_coordinates = {0, 0}; // dummy coordinates
    record.material = material.get();
    return record;
}

template <Dimension DIMENSION>
requires(DIMENSION >= 3) std::optional<
    AxisAlignedBoundingBox<DIMENSION>> Sphere<DIMENSION>::bounding_box()
const {
    Vec<DIMENSION> corner{};
    std::ranges::fill(corner.coefficients, radius);
    return AxisAlignedBoundingBox<DIMENSION>{position - corner,
                                             position + corner};
}

}} // namespace cpp_raytracing::cartesian_embedded

#endif
