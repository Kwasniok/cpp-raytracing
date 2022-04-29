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
    /** @brief configure the camera based on real world paramerters */
    constexpr void configure(const Scalar vertical_field_of_view_deg,
                             const Scalar aspect_ratio) {

        const auto theta = rad_from_deg(vertical_field_of_view_deg);
        const auto viewport_height = 2 * std::tan(theta / 2.0);
        const auto viewport_width = aspect_ratio * viewport_height;

        const Scalar focal_length = 1.0;

        direction_x = Vec3{viewport_width / 2.0, 0.0, 0.0};
        direction_y = Vec3{0.0, viewport_height / 2.0, 0.0};
        direction_z = Vec3{0.0, 0.0, -focal_length};
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
    /** @brief canvas width in pixel */
    unsigned long canvas_width = 128;
    /** @brief canvas height in pixel */
    unsigned long canvas_height = 128;
    /** @brief position of camera in space */
    Vec3 origin{};
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
