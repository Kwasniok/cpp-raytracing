/**
 * @file
 * @brief camera in Minkowski space
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_CAMERA_HPP
#define CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_CAMERA_HPP

#include "../../world/entities/camera.hpp"
#include "geometry.hpp"

namespace cpp_raytracing { namespace minkowski {

/**
 * @brief returns a pin hole camera for the Minkowski geometry
 * @note All rays start at the detector surface at the same time and pass
 * through the pinhole.
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

/**
 * @brief represents the equivalent of a traditional camera with perspective
 *        projection
 * @note All rays start at the same postion in space-time.
 */
class PerspectiveCamera : public Camera<4_D> {
  public:
    /** @brief position of camera  */
    Vec<3_D> position{};
    /** @brief time */
    Scalar time = 0;

    /** @brief detector x direction */
    Vec<3_D> direction_u{1, 0, 0};
    /** @brief detector y direction */
    Vec<3_D> direction_v{0, 1, 0};
    /** @brief detector z direction */
    Vec<3_D> direction_z{0, 0, -1};

    /** @see Entity::set_time   */
    void set_time(const Scalar time) override {
        Camera<4_D>::set_time(time);
        this->time = time;
    }

    /** @see Camera::ray_for_coords */
    std::unique_ptr<cpp_raytracing::Ray<4_D>>
    ray_for_coords(const cpp_raytracing::Geometry<4_D>& geometry,
                   const Scalar x, const Scalar y) const override;
};

std::unique_ptr<cpp_raytracing::Ray<4_D>>
PerspectiveCamera::ray_for_coords(const cpp_raytracing::Geometry<4_D>& geometry,
                                  const Scalar x, const Scalar y) const {
    using namespace tensor;

    const minkowski::Geometry& geo =
        dynamic_cast<const minkowski::Geometry&>(geometry);

    const Vec<4_D> start =
        embeded_vector<4_D, 0, 3>(position) + time * base_vec<4_D, 3>;
    const Vec<3_D> direction = direction_z + direction_u * x + direction_v * y;

    return std::make_unique<Ray>(geo.speed_of_light, start, direction);
}

/**
 * @brief returns a camera for the Minkowski geometry
 * @note All rays start at the same postion in space-time.
 * @see cpp_raytracing::geometries::minkowski::Geometry
 */
PerspectiveCamera make_perspective_camera(
    const Vec3 position, const Vec3 focus, const Vec3 up_direction,
    const Scalar vertical_field_of_view_deg, const Scalar aspect_ratio) {
    using namespace tensor;

    const auto theta = rad_from_deg(vertical_field_of_view_deg);
    const auto viewport_height = 2 * std::tan(theta / 2.0);
    const auto viewport_width = aspect_ratio * viewport_height;

    const auto w = unit_vector(focus - position);
    const auto u = unit_vector(cross(up_direction, w));
    const auto v = cross(u, w);
    const Scalar focus_distance = length(focus - position);

    const auto x = focus_distance * (viewport_width / 2.0) * u;
    const auto y = focus_distance * (viewport_height / 2.0) * v;
    const auto z = focus_distance * w;

    PerspectiveCamera camera;
    camera.position = position;
    camera.direction_u = x;
    camera.direction_v = y;
    camera.direction_z = z;

    return camera;
}

}} // namespace cpp_raytracing::minkowski

#endif
