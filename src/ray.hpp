#ifndef CPP_RAYTRACING_RAY_H
#define CPP_RAYTRACING_RAY_H

#include "vec3.hpp"

namespace ray {

class Ray {
  public:
    constexpr Ray(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction) {}

    constexpr Vec3 start() const { return _start; }
    constexpr Vec3 direction() const { return _direction; }

    constexpr Vec3 at(const Scalar t) const { return _start + t * _direction; }

    constexpr bool direction_near_zero(const Scalar epsilon) const {
        return _direction.near_zero(epsilon);
    }

  private:
    Vec3 _start;
    Vec3 _direction;
};

} // namespace ray

#endif
