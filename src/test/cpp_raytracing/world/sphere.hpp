/**
 * @file
 * @brief hittable spherical object
 */

#ifndef CPP_RAYTRACING_TEST_ENTITIES_SPHERE_HPP
#define CPP_RAYTRACING_TEST_ENTITIES_SPHERE_HPP

#include <cmath>

#include <cpp_raytracing/world/entities/base.hpp>

namespace cpp_raytracing {

/**
 * @brief spherical object
 * @note Asserts Euclidean Geometry and ignores geometry parameter.
 */
class Sphere3D : public Entity3D {
  public:
    /**
     * @brief radius
     * @note Negative radii relate to spheres with the inside out. This is
     *       usefull for the inner surface of glass etc.
     */
    Scalar radius = 1.0;
    /** @brief material of the sphere */
    std::shared_ptr<Material> material;

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

    HitRecord hit_record(const Geometry& geometry,
                         const RaySegment3D& ray_segment,
                         const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox3D> bounding_box() const override;

  private:
    /**
     * @brief transforms normal vector of a sphere to uv coordinates
     * @note `u = phi / (2 * pi)`, `v = theta / pi`, y = -cos(theta)`, `x =
     *       -sin(theta) * cos(phi)` and `z = sin(theta) * sin(phi)`.
     * @note This uv mapping is similar but not identical to the canonical
     *       mappings in maths and physics.
     */
    static Vec2 uv_coordinates(const Vec3& normal) {
        const auto theta = std::acos(-normal[1]);
        const auto phi = std::atan2(-normal[2], normal[0]) + pi;

        return {phi / (2.0 * pi), theta / pi};
    }
};

HitRecord Sphere3D::hit_record([[maybe_unused]] const Geometry& geometry,
                               const RaySegment3D& ray_segment,
                               const Scalar t_min) const {
    using namespace tensor;

    // analytical geometry: line hits sphere
    // ray: s + t*d
    // sphere: (x-o)^2 = R^2
    // solve: a*t^2 + b*t + c = 0
    // where a = d^2 >= 0, b = 2*d*(s-o), c = (s-o)^2 - R^2
    // solution: t = (-b +/- sqrt(b^2 - 4ac))/(2a)
    const auto delta = ray_segment.start();
    const auto a = dot(ray_segment.direction(), ray_segment.direction());
    const auto b_half = dot(ray_segment.direction(), delta);
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
    if (t < t_min || t > ray_segment.t_max()) {
        t = (-b_half + sqrt(discriminant)) / a;
        if (t < t_min || t > ray_segment.t_max()) {
            // no soltion in range
            return HitRecord{.t = infinity};
        }
    }

    // found solution in range
    const Vec3 point = ray_segment.at(t);
    const Vec3 normal = point * (Scalar{1} / radius);
    HitRecord record;
    record.t = t;
    record.point = point;
    record.set_face_normal(tensor::identity_mat<3_D>, tensor::identity_mat<3_D>,
                           ray_segment.direction(), normal);
    record.uv_coordinates = uv_coordinates(normal);
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox3D> Sphere3D::bounding_box() const {
    return AxisAlignedBoundingBox3D{-Vec3{radius, radius, radius},
                                    Vec3{radius, radius, radius}};
}

/** @brief make a 3D sphere */
std::unique_ptr<Instance3D> make_sphere(const Vec3 position,
                                        const Scalar radius) {
    auto sphere = std::make_shared<Sphere3D>();
    sphere->radius = radius;

    auto instance = std::make_unique<Instance3D>();
    instance->entity = sphere;
    instance->position = position;

    return instance;
}

} // namespace cpp_raytracing

#endif
