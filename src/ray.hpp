/**
 * @file
 * @brief light ray
 */

#ifndef CPP_RAYTRACING_RAY_H
#define CPP_RAYTRACING_RAY_H

#include "vec3.hpp"

namespace ray {

/**
 * @brief representation of a light ray
 */
class Ray {
  public:
    /** @brief initialize with parameters */
    constexpr Ray(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction) {}

    /** @brief starting point of the ray */
    constexpr Vec3 start() const { return _start; }
    /** @brief direction of the ray */
    constexpr Vec3 direction() const { return _direction; }

    /**
     * @brief point on the ray for given parameter
     * @return `start() + t * direction()`
     */
    constexpr Vec3 at(const Scalar t) const { return _start + t * _direction; }

    /** @brief tests if direction of ray is a zero vector */
    constexpr bool direction_near_zero(const Scalar epsilon) const {
        return _direction.near_zero(epsilon);
    }

  private:
    Vec3 _start;
    Vec3 _direction;
};

} // namespace ray

#endif
