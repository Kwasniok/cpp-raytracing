/**
 * @file
 * @brief hittable mesh object
 */

#ifndef CPP_RAYTRACING_ENTITIES_MESH_HPP
#define CPP_RAYTRACING_ENTITIES_MESH_HPP

#include <array>
#include <memory>
#include <vector>

#include "../../values/tensor.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief mesh object
 * @note Each face is filled via bilinear coordinate interpolation.
 */
class Mesh : public Entity {
  public:
    /** @brief point index type */
    using Index = std::vector<Vec3>::size_type;
    /** @brief face type (triplet of point indices)*/
    using Face = std::array<Index, 3>;

    /** @brief face corners */
    std::vector<Vec3> points;
    /** @brief faces */
    std::vector<Face> faces;
    /** @brief material of the plane */
    std::shared_ptr<Material> material;

    virtual ~Mesh() = default;

    virtual void set_time(const Scalar time) override;

    virtual HitRecord hit_record(const Geometry& geometry,
                                 const RaySegment& ray_segment,
                                 const Scalar t_min = 0.0) const override;

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

    HitRecord face_hit_record(const Geometry& geometry,
                              const RaySegment& ray_segment, const Scalar t_min,
                              const Vec3& point0, const Vec3& point1,
                              const Vec3& point2) const {
        // basis for span
        const Vec3 b1 = point1 - point0;
        const Vec3 b2 = point2 - point0;
        // pseudo-normal (NOT the face normal)
        const Vec3 n = unit_vector(cross(b1, b2));
        // level parameter
        const Scalar l = dot(n, point0);

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

        const auto [u, v] = uv_tri_coords(b1, b2, (s + t * d) - point0);

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
        normal = normal / (dot(normal, metric * normal));
        record.set_face_normal(to_onb_jacobian, metric, d, normal);
        record.material = material.get();
        return record;
    }

    void generate_cache() {
        constexpr Scalar epsilon = 1e-8;

        Vec3 low = {+infinity, +infinity, +infinity};
        Vec3 high = {-infinity, -infinity, -infinity};

        for (const auto& point : points) {
            low = elementwise<min>(low, point);
            high = elementwise<max>(high, point);
        }

        // padding to guarantee non-zero volume
        low -= elementwise<abs>(low) * epsilon;
        high += elementwise<abs>(high) * epsilon;

        _bounds = {low, high};
    }

  private:
    // note: initial value is irrelevant
    AxisAlignedBoundingBox _bounds = {Vec3::zero(), Vec3::zero()};
};

/**
 * @brief face type (triplet of point indices)
 * @see Mesh
 */
using Face = Mesh::Face;

void Mesh::set_time(const Scalar time) {
    Entity::set_time(time);
    generate_cache();
}

HitRecord Mesh::hit_record(const Geometry& geometry,
                           const RaySegment& ray_segment,
                           const Scalar t_min) const {
    HitRecord record = {.t = infinity};
    for (const auto& face : faces) {
        HitRecord rec =
            face_hit_record(geometry, ray_segment, t_min, points[face[0]],
                            points[face[1]], points[face[2]]);
        if (rec.t < record.t) {
            record = rec;
        }
    }
    return record;
}

std::optional<AxisAlignedBoundingBox> Mesh::bounding_box() const {
    return {_bounds};
}

} // namespace cpp_raytracing

#endif
