/**
 * @file
 * @brief hittable triangular object
 */

#ifndef CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP
#define CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP

#include <utility>

#include "../../values/tensor.hpp"
#include "base.hpp"

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

    /** @brief default constructor */
    Triangle() = default;

    /** @brief copy constructor */
    Triangle(const Triangle&) = delete;

    /** @brief move constructor */
    Triangle(Triangle&&) = default;

    /** @brief copy assignment */
    Triangle& operator=(const Triangle&) = delete;

    /** @brief move assignment */
    Triangle& operator=(Triangle&&) = default;

    ~Triangle() override = default;

    HitRecord hit_record(const Geometry& geometry,
                         const RaySegment3D& ray_segment,
                         const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox3D> bounding_box() const override;

  private:
    /**
     * @brief returns u-v coordinates of x in plane spanned by b1 and b2
     * @note It is asserted that x is in the plane spanned by b1 and b2.
     * @note `x = u * b1 + v * b2`
     * @note This is solving for linear coefficients.
     */
    static constexpr std::pair<Scalar, Scalar>
    uv_tri_coords(const Vec3& b1, const Vec3& b2, const Vec3& x) {
        using namespace tensor;

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

HitRecord Triangle::hit_record(const Geometry& geometry,
                               const RaySegment3D& ray_segment,
                               const Scalar t_min) const {
    using namespace tensor;

    // basis for span
    const Vec3 b1 = points[1] - points[0];
    const Vec3 b2 = points[2] - points[0];
    // pseudo-normal (NOT the face normal)
    const Vec3 n = unit_vector(cross(b1, b2));
    // level parameter
    const Scalar l = dot(n, points[0]);

    // ray
    const Vec3 s = ray_segment.start();
    const Vec3 d = ray_segment.direction();

    // intersection parameters
    const Scalar a = l - dot(s, n);
    const Scalar b = dot(d, n);

    if (b == 0.0) {
        // ray 'parallel' to face
        return {.t = infinity};
    }

    const Scalar t = a / b;

    if (t < t_min || t >= ray_segment.t_max()) {
        // outside of ray segment
        return {.t = infinity};
    }

    const auto [u, v] = uv_tri_coords(b1, b2, (s + t * d) - points[0]);

    if (u < 0.0 || v < 0.0 || u + v > 1.0) {
        // outside of triangle region
        return {.t = infinity};
    }

    // construct hit record
    const Vec3 point = ray_segment.at(t);
    const Mat3x3 metric = geometry.metric(point);
    const Mat3x3 to_onb_jacobian = geometry.to_onb_jacobian(point);

    HitRecord record;
    record.t = t;
    record.point = point;
    record.uv_coordinates = {u, v};
    // note: The normal is position dependent since the tri might be curved.
    Vec3 normal = cross(metric * b1, metric * b2);
    // normalize
    normal *= Scalar{1} / std::sqrt(dot(normal, metric * normal));
    record.set_face_normal(to_onb_jacobian, metric, d, normal);
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox3D> Triangle::bounding_box() const {

    constexpr Scalar epsilon = 1e-8;

    Vec3 low = {+infinity, +infinity, +infinity};
    Vec3 high = {-infinity, -infinity, -infinity};

    for (const auto& point : points) {
        low.inplace_elementwise(min, point);
        high.inplace_elementwise(max, point);
    }

    // padding to guarantee non-zero volume
    low -= low.elementwise(abs) * epsilon;
    high += high.elementwise(abs) * epsilon;

    return AxisAlignedBoundingBox3D{low, high};
}

} // namespace cpp_raytracing

#endif
