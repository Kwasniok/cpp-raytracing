/**
 * @file
 * @brief light ray
 */

#ifndef CPP_RAYTRACING_RAY_HPP
#define CPP_RAYTRACING_RAY_HPP

#include <optional>

#include "../values/tensor.hpp"

namespace cpp_raytracing {

/**
 * @brief representation of a light ray segment
 */
class RaySegment {
  public:
    /**
     * @brief initialize with parameters
     * @param start starting point of ray segment
     * @param direction direction vector must be either zero or finite vector
     * @param t_max end of segment (must be strctly positive and may be
     *        infinity)
     */
    constexpr RaySegment(const Vec3& start, const Vec3& direction,
                         const Scalar t_max = infinity)
        : _start(start), _direction(direction), _t_max(t_max) {}

    /** @brief starting point of the ray */
    constexpr Vec3 start() const { return _start; }
    /** @brief direction of the ray */
    constexpr Vec3 direction() const { return _direction; }
    /** @brief maximal value for parameter t */
    constexpr Scalar t_max() const { return _t_max; }

    /**
     * @brief returns true iff parameter denotes a point within this current
     *        segment
     */
    constexpr bool contains(const Scalar t) const { return t < _t_max; }

    /**
     * @brief point on the ray for given parameter
     * @return `start() + t * direction()`
     */
    constexpr Vec3 at(const Scalar t) const { return _start + t * _direction; }

    /** @brief tests if direction of ray is a zero vector */
    constexpr bool direction_near_zero(const Scalar epsilon) const {
        return _direction.near_zero(epsilon);
    }

    /** @brief tests if direction of ray is a perfect zero vector */
    constexpr bool direction_exactly_zero() const {
        return _direction[0] == 0.0 && _direction[1] == 0.0 &&
               _direction[2] == 0.0;
    }

  private:
    Vec3 _start;
    Vec3 _direction;
    Scalar _t_max;
};

} // namespace cpp_raytracing

#endif
