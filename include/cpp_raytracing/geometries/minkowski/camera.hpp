/**
 * @file
 * @brief camera in Minkowski space
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_CAMERA_HPP
#define CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_CAMERA_HPP

#include "../../world/entities/camera.hpp"

namespace cpp_raytracing { namespace minkowski {

/**
 * @brief returns a pin hole camera for the Minkowski geometry
 * @see cpp_raytracing::geometries::minkowski::Geometry
 */
PinholeCamera<4_D> make_pinhole_camera(const Vec3 detector_origin,
                                       const Vec3 pinhole,
                                       const Vec3 up_direction,
                                       const Scalar vertical_field_of_view_deg,
                                       const Scalar aspect_ratio) {
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
        embeded_vector<4_D, 0, 3>(detector_origin + z),
        // clang-format off
        [
            origin = embeded_vector<4_D, 0, 3>(detector_origin),
            x = embeded_vector<4_D, 0, 3>(x),
            y = embeded_vector<4_D, 0, 3>(y),
            t = base_vec<4_D, 3>
        ]
        // clang-format on
        (const Scalar u, const Scalar v, const Scalar time) {
            return origin + u * x + v * y + time * t;
        },
    };
}

}} // namespace cpp_raytracing::minkowski

#endif
