/**
 * @file
 * @brief camera in Cartesian Schwarzschild space
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_CAMERA_HPP
#define CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_CAMERA_HPP

#include "../../world/entities/camera.hpp"
#include "geometry.hpp"

namespace cpp_raytracing { namespace schwarzschild {

/**
 * @brief represents the equivalent of a traditional camera with perspective
 *        projection
 * @note To be used in almot flat regions only.
 */
class PerspectiveCamera : public Camera<4_D> {
  public:
    /** @brief position of camera  */
    Vec<4_D> position{};
    /** @brief time */
    Scalar time = 0;

    /** @brief detector x direction */
    Vec<4_D> direction_u{1, 0, 0, 0};
    /** @brief detector y direction */
    Vec<4_D> direction_v{0, 1, 0, 0};
    /** @brief detector z direction */
    Vec<4_D> direction_z{0, 0, -1, 0};
    /** @brief time direction */
    constexpr static Vec<4_D> direction_t{0, 0, 0, 1};

    /** @see Entity::set_time   */
    void set_time(const Scalar time) override {
        Camera<4_D>::set_time(time);
        this->time = time;
    }

    /** @see Camera::ray_for_coords */
    std::unique_ptr<Ray<4_D>>
    ray_for_coords(const cpp_raytracing::Geometry<4_D>& geometry,
                   const Scalar x, const Scalar y) const override;
};

std::unique_ptr<Ray<4_D>>
PerspectiveCamera::ray_for_coords(const cpp_raytracing::Geometry<4_D>& geometry,
                                  const Scalar x, const Scalar y) const {

    const schwarzschild::Geometry& geo =
        dynamic_cast<const schwarzschild::Geometry&>(geometry);

    const Vec<4_D> start = position + time * direction_t;
    const Vec<4_D> direction = direction_z + direction_u * x + direction_v * y -
                               (1.0 / geo.speed_of_light()) * direction_t;

    return std::make_unique<RungeKuttaRay<4_D>>(geo, start, direction);
}

/**
 * @brief returns a camera for the Cartesian Schwarzschild geometry
 * @note For regions far from black hole/origin only.
 * @see cpp_raytracing::schwarzschild::Geometry
 */
PerspectiveCamera make_far_perspective_camera(
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
    camera.position = embeded_vector<4_D, 0, 3>(position);
    camera.direction_u = embeded_vector<4_D, 0, 3>(x);
    camera.direction_v = embeded_vector<4_D, 0, 3>(y);
    camera.direction_z = embeded_vector<4_D, 0, 3>(z);

    return camera;
}

}} // namespace cpp_raytracing::schwarzschild

#endif
