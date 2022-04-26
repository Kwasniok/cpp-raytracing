/**
 * @file
 * @brief camera
 */

#ifndef CPP_RAYTRACING_CAMERA_HPP
#define CPP_RAYTRACING_CAMERA_HPP

#include "ray.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief represents camera
 */
class Camera {
  public:
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
    Vec3 direction_x{};
    /**
     * @brief direction of image height in space
     * @note A longer vector will compress the image.
     */
    Vec3 direction_y{};
    /**
     * @brief direction of image depth in space
     * @note The vector length is the focus distance (where the focal plane is).
     */
    Vec3 direction_z{};
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
