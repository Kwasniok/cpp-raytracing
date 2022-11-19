/**
 * @file
 * @brief camera in n-dimensional embedded Cartesian manifolds
 * @note To be used for testing and debugging.
 */

#ifndef CPP_RAYTRACING_GEOMETRY_CARTESIAN_EMBEDDED_CAMERA_HPP
#define CPP_RAYTRACING_GEOMETRY_CARTESIAN_EMBEDDED_CAMERA_HPP

#include "../../world/entities/camera.hpp"

namespace cpp_raytracing {

/**
 * @brief pinhole camera from conventional camera parameters for an
 *        n-dimensional embedded cartesian coordinate system and (locally)
 *        Euclidean space
 * @note For convenience, the image is flipped vertically unlike real pinhole
 *       images.
 * @note To be used for testing and debugging.
 */
template <Dimension DIMENSION>
PinholeCamera<DIMENSION> make_pinhole_camera_cartesian_embedded_euclidean(
    const Vec3 detector_origin, const Vec3 pinhole, const Vec3 up_direction,
    const Scalar vertical_field_of_view_deg, const Scalar aspect_ratio,
    const Vec<DIMENSION> time_direction = {}) {
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

    return {
        embeded_vector<DIMENSION, 0, 3>(detector_origin + z),
        // clang-format off
        [
            origin = embeded_vector<DIMENSION, 0, 3>(detector_origin),
            x = embeded_vector<DIMENSION, 0, 3>(x),
            y = embeded_vector<DIMENSION, 0, 3>(y),
            time_direction = time_direction
        ]
        // clang-format on
        (const Scalar u, const Scalar v, const Scalar time) {
            return origin + u * x + v * y + time * time_direction;
        },
    };
}

} // namespace cpp_raytracing

#endif
