/**
 * @file
 * @brief hittable triangular object
 */

#ifndef CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP
#define CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP

#include <utility>

#include "../../values/tensor.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief triangular object
 * @note The face is filled via bilinear coordinate interpolation.
 */
class Triangle : public Entity {
  public:
    /** @brief face corners */
    std::array<Vec3, 3> points;
    /** @brief material of the plane */
    std::shared_ptr<Material> material;

    virtual ~Triangle() = default;

    virtual HitRecord hit_record(const Geometry& geometry,
                                 const RaySegment& ray,
                                 const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

  private:
    /**
     * @brief returns u-v coordinates of x in plane spanned by b1 and b2
     * @note It is asserted that x is in the plane spanned by b1 and b2.
     * @note `x = u * b1 + v * b2`
     * @note This is solving for linear coefficients.
     */
    static constexpr std::pair<Scalar, Scalar>
    uv_tri_coords(const Vec3& b1, const Vec3& b2, const Vec3& x) {
        const Scalar b1b1 = dot(b1, b1);
        const Scalar b1b2 = dot(b1, b2);
        const Scalar b2b2 = dot(b2, b2);
        const Scalar D = b1b1 * b2b2 - b1b2 * b1b2;
        const Scalar b1x = dot(b1, x);
        const Scalar b2x = dot(b2, x);
        const Scalar u = (b1b1 * b2x - b1b2 * b1x) / D;
        const Scalar v = (b2b2 * b1x - b1b2 * b2x) / D;
        return {u, v};
    }
};

HitRecord Triangle::hit_record(const Geometry& geometry, const RaySegment& ray,
                               const Scalar t_min, const Scalar t_max) const {

    // basis for span
    const Vec3 b1 = points[1] - points[0];
    const Vec3 b2 = points[2] - points[0];
    // pseudo-normal (NOT the face normal)
    const Vec3 n = unit_vector(cross(b1, b2));
    // level parameter
    const Scalar l = dot(n, points[0]);

    // ray
    const Vec3 s = ray.start();
    const Vec3 d = ray.direction();

    // intersection parameters
    const Scalar a = l - dot(s, n);
    const Scalar b = dot(d, n);

    if (b == 0.0) {
        // ray 'parallel' to face
        return {.t = infinity};
    }

    const Scalar t = a / b;

    if (t < 0.0 || t >= t_max) {
        // outside of ray segment
        return {.t = infinity};
    }

    const auto [u, v] = uv_tri_coords(b1, b2, (s + t * d) - points[0]);

    if (u < 0.0 || v < 0.0 || u + v > 1.0) {
        // outside of triangle region
        return {.t = infinity};
    }

    // TODO:
    HitRecord record;
    record.t = t;
    record.point = ray.at(t);
    record.metric = geometry.metric(record.point);
    record.uv_coordinates = {u, v};
    // note: The normal is position dependent since the tri might be curved.
    Vec3 normal = cross(record.metric * b1, record.metric * b2);
    // normalize
    normal = normal / (dot(normal, record.metric * normal));
    record.set_face_normal(record.metric, ray.direction(), normal);
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox> Triangle::bounding_box() const {

    constexpr Scalar epsilon = 1e-8;

    Scalar x_min = std::min(points[0][0], std::min(points[1][0], points[2][0]));
    Scalar y_min = std::min(points[0][1], std::min(points[1][1], points[2][1]));
    Scalar z_min = std::min(points[0][2], std::min(points[1][2], points[2][2]));

    Scalar x_max = std::max(points[0][0], std::max(points[1][0], points[2][0]));
    Scalar y_max = std::max(points[0][1], std::max(points[1][1], points[2][1]));
    Scalar z_max = std::max(points[0][2], std::max(points[1][2], points[2][2]));

    // padding to guarantee non-zero volume
    x_min -= std::abs(x_min) * epsilon;
    y_min -= std::abs(y_min) * epsilon;
    z_min -= std::abs(z_min) * epsilon;

    x_max += std::abs(x_max) * epsilon;
    y_max += std::abs(y_max) * epsilon;
    z_max += std::abs(z_max) * epsilon;

    return AxisAlignedBoundingBox{
        Vec3{x_min, y_min, z_min},
        Vec3{x_max, y_max, z_max},
    };
}

} // namespace cpp_raytracing

#endif
