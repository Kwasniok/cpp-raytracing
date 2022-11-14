/**
 * @file
 * @brief camera in 3D Cartesian manifolds
 * @note To be used in (alomst) flat regions of the geometry only-
 */

#ifndef CPP_RAYTRACING_GEOMETRY_CARTESIAN_3D_CAMERA_HPP
#define CPP_RAYTRACING_GEOMETRY_CARTESIAN_3D_CAMERA_HPP

#include "../../world/entities/camera.hpp"

namespace cpp_raytracing {

/**
 * @brief pinhole camera from conventional camera parameters for a 3D cartesian
 *        coordinate system and (locally) Euclidean space
 * @note For convenience, the image is flipped vertically unlike real pinhole
 *       images.
 */
PinholeCamera3D make_pinhole_camera_cartesian_euclidean_3d(
    const Vec3 detector_origin, const Vec3 pinhole, const Vec3 up_direction,
    const Scalar vertical_field_of_view_deg, const Scalar aspect_ratio) {
    using namespace tensor;

    const auto theta = rad_from_deg(vertical_field_of_view_deg);
    const auto viewport_height = 2 * std::tan(theta / 2.0);
    const auto viewport_width = aspect_ratio * viewport_height;

    const auto w = unit_vector(pinhole - detector_origin);
    const auto u = unit_vector(cross(up_direction, w));
    const auto v = cross(u, w);
    const Scalar focus_distance = length(pinhole - detector_origin);

    const auto x = focus_distance * (viewport_width / 2.0) * u;
    const auto y = focus_distance * (viewport_height / 2.0) * v;
    const auto z = focus_distance * w;

    return {detector_origin + z, [origin = detector_origin, x = x,
                                  y = y](const Scalar u, const Scalar v) {
                return origin + u * x + v * y;
            }};
}

} // namespace cpp_raytracing

#endif
