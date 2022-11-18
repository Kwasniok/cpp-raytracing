/**
 * @file
 * @brief hittable mesh object consisting  of (small) triangles in a 3D manifold
 */

#ifndef CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_MESH_HPP
#define CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_MESH_HPP

#include "mesh.hpp"

namespace cpp_raytracing {

/**
 * @brief mesh object of (small) triangles in a 3D manifold
 * @note Each face is filled via bilinear coordinate interpolation.
 * @note 'small' means there is no significant deviation from flat space across
 *       each individual triangle. This means coordinate deltas and local
 *       tangential vectors are approxiately proportional within each
 *       individual triangle.
 */
class SmallTriangleMesh3D : public Mesh3D {
  public:
    /** @brief default constructor */
    SmallTriangleMesh3D() = default;

    /** @brief copy constructor */
    SmallTriangleMesh3D(const SmallTriangleMesh3D&) = delete;

    /** @brief move constructor */
    SmallTriangleMesh3D(SmallTriangleMesh3D&&) = default;

    /** @brief copy assignment */
    SmallTriangleMesh3D& operator=(const SmallTriangleMesh3D&) = delete;

    /** @brief move assignment */
    SmallTriangleMesh3D& operator=(SmallTriangleMesh3D&&) = default;

    ~SmallTriangleMesh3D() override = default;

    /**
     * @see Entity::hit_record
     * @note Works for 'small' triangles only - i.e. there MUST NOT be any
     *       significant curvature across each triangle of the mesh.
     */
    HitRecord3D hit_record(const Geometry3D& geometry,
                           const RaySegment3D& ray_segment,
                           const Scalar t_min = 0.0) const override;

  private:
    HitRecord3D face_hit_record(const Geometry3D& geometry,
                                const RaySegment3D& ray_segment,
                                const Scalar t_min, const Vec3& point0,
                                const Vec3& point1, const Vec3& point2) const {

        using namespace tensor;
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

        const auto [u, v] =
            get_coords_in_plane<3_D>(b1, b2, (s + t * d) - point0).coefficients;

        if (u < 0.0 || v < 0.0 || u + v > 1.0) {
            // outside of triangle region
            return {.t = infinity};
        }

        // construct hit record
        const Vec3 point = ray_segment.at(t);
        const Mat3x3 metric = geometry.metric(point);
        const Mat3x3 to_onb_jacobian = geometry.to_onb_jacobian(point);

        HitRecord3D record;
        record.t = t;
        record.point = point;
        record.uv_coordinates = {u, v};
        // note: The normal is position dependent since the tri might be curved.
        // note: Calculating a face normal from the cross product of two
        //       coordinate deltas (as given here) works only, if the curvature
        //       deltas are approximately the same as the local tangent vectors
        //       in the sme direction. This is only true, if the CURVATURE AROSS
        //       THE SURFACE is NOT SIGNIFICANT!
        //       In general, the local tangential vectors t1 and t2 are the
        //       local tangents of the two geodesics running through the hit
        //       point and `point1` and `pont2` respectively.
        Vec3 normal = cross(metric * b1, metric * b2);
        // normalize
        normal *= Scalar{1} / std::sqrt((dot(normal, metric * normal)));
        record.set_local_geometry(to_onb_jacobian * d,
                                  to_onb_jacobian * normal);
        record.material = this->material.get();
        return record;
    }
};

HitRecord3D SmallTriangleMesh3D::hit_record(const Geometry3D& geometry,
                                            const RaySegment3D& ray_segment,
                                            const Scalar t_min) const {
    HitRecord3D record = {.t = infinity};
    for (const auto& face : this->faces) {
        HitRecord3D rec =
            face_hit_record(geometry, ray_segment, t_min, this->points[face[0]],
                            this->points[face[1]], this->points[face[2]]);
        if (rec.t < record.t) {
            record = rec;
        }
    }
    return record;
}

} // namespace cpp_raytracing

#endif
