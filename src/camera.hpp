#ifndef CPP_RAYTRACING_CAMERA_H
#define CPP_RAYTRACING_CAMERA_H

#include "ray.hpp"
#include "vec3.hpp"

namespace ray {

class Camera {
  public:
    Ray ray_for_coords(const Scalar x, const Scalar y) const {
        Vec3 random_vec = lens_radius * random_in_unit_disk();
        Vec3 defocus_offset =
            direction_x * random_vec.x() + direction_y * random_vec.y();
        return Ray(origin + defocus_offset, direction_z + x * direction_x +
                                                y * direction_y +
                                                -defocus_offset);
    }

  public:
    unsigned long canvas_width = 0;
    unsigned long canvas_height = 0;
    Vec3 origin{};
    Vec3 direction_x{};
    Vec3 direction_y{};
    Vec3 direction_z{};
    Scalar lens_radius = 0.0;
};

} // namespace ray

#endif
