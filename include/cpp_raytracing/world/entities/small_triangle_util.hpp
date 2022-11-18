/**
 * @file
 * @brief utility for small triangle entities
 */

#ifndef CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_UTIL_HPP
#define CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_UTIL_HPP

#include "../../values/tensor.hpp"
#include "../materials/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/** @see Entity::hit_record
 * @note Works for 'small' triangles only - i.e. there MUST NOT be any
 *       significant curvature across each triangle of the mesh.
 */
inline HitRecord3D small_triangle_hit_record_3d(
    const Geometry3D& geometry, const RaySegment3D& ray_segment,
    const Scalar t_min, const Vec3& point0, const Vec3& point1,
    const Vec3& point2, const Material3D* material) {
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

    // union access is checked by gttl
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const auto [u, v] =
        get_coords_in_plane<3_D>(b1, b2, (s + t * d) - point0).coefficients;
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

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
    normal *= Scalar{1} / std::sqrt(dot(normal, metric * normal));
    record.set_local_geometry(to_onb_jacobian * d, to_onb_jacobian * normal);
    record.material = material;
    return record;
}

} // namespace cpp_raytracing

#endif