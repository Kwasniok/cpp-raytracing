/**
 * @file
 * @brief camera
 */

#ifndef CPP_RAYTRACING_CAMERA_HPP
#define CPP_RAYTRACING_CAMERA_HPP

#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief represents camera
 */
class Camera {
  public:
    /**
     * @brief configure the camera based on real world paramerters
     * @param look_from position of the camera
     * @param look_at center of the focal plane
     * @param look_up orientation of the camera's up/y-direction
     * @param vertical_field_of_view_deg vertical field of view in degree
     * @param aspect_ratio ratio of viewport width to viewport height
     * @param aperature size of the camera's aperature (arbitrary units)
     */
    constexpr Camera(const Vec3 look_from, const Vec3 look_at,
                     const Vec3 look_up,
                     const Scalar vertical_field_of_view_deg,
                     const Scalar aspect_ratio, const Scalar aperature) {

        const auto theta = rad_from_deg(vertical_field_of_view_deg);
        const auto viewport_height = 2 * std::tan(theta / 2.0);
        const auto viewport_width = aspect_ratio * viewport_height;

        const auto w = -unit_vector(look_at - look_from);
        const auto u = unit_vector(cross(w, look_up));
        const auto v = cross(u, w);
        const Scalar focal_length = (look_at - look_from).length();

        origin = look_from;
        direction_x = (viewport_width / 2.0) * u;
        direction_y = (viewport_height / 2.0) * v;
        direction_z = focal_length * w;
        lens_radius = aperature / 2.0;
    }

    /** @brief calculates ray for pixel coordinates of canvas */
    Ray ray_for_coords(const Scalar x, const Scalar y) const {
        Vec3 random_vec = lens_radius * random_in_unit_disk();
        Vec3 defocus_offset =
            direction_x * random_vec.x() + direction_y * random_vec.y();
        return Ray(origin + defocus_offset, direction_z + x * direction_x +
                                                y * direction_y +
                                                -defocus_offset);
    }

  public:
    /** @brief position of camera in space */
    Vec3 origin{0.0, 0.0, 0.0};
    /**
     * @brief direction of image width in space
     * @note A longer vector will compress the image.
     */
    Vec3 direction_x{1.0, 0.0, 0.0};
    /**
     * @brief direction of image height in space
     * @note A longer vector will compress the image.
     */
    Vec3 direction_y{0.0, 1.0, 0.0};
    /**
     * @brief direction of image depth in space
     * @note The vector length is the focus distance (where the focal plane is).
     */
    Vec3 direction_z{0.0, 0.0, -1.0};
    /**
     * @brief size of the lens radius
     * @note A wider radius leads to an increased defocus and a thighter focus
     * around the focal plane.
     * @note `lens_radius = aperature / 2`
     */
    Scalar lens_radius = 0.0;
};

} // namespace cpp_raytracing

#endif
