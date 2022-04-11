#ifndef CPP_RAYTRACING_CAMERA_H
#define CPP_RAYTRACING_CAMERA_H

#include "ray.hpp"
#include "vec3.hpp"

namespace ray {

class Camera {
  public:
    Ray ray_for_coords(const Scalar x, const Scalar y) const {
        return Ray(origin, direction_z + x * direction_x + y * direction_y);
    }

  public:
    long canvas_width = 0;
    long canvas_height = 0;
    Vec3 origin{};
    Vec3 direction_x{};
    Vec3 direction_y{};
    Vec3 direction_z{};
};

} // namespace ray

#endif
