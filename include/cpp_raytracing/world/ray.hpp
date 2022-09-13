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
     * @param proper_length_factor ratio of proper length of direction in curved
     *        space vector to length of direction()
     */
    constexpr RaySegment(const Vec3& start, const Vec3& direction,
                         const Scalar proper_length_factor = 1.0)
        : _start(start),
          _direction(direction),
          _proper_length_factor(proper_length_factor) {}

    /** @brief starting point of the ray */
    constexpr Vec3 start() const { return _start; }
    /** @brief direction of the ray */
    constexpr Vec3 direction() const { return _direction; }
    /**
     * @brief ratio of proper length of direction in curved space vector to
     *        length of direction()
     */
    constexpr Scalar proper_length_factor() const {
        return _proper_length_factor;
    }
    /** @brief proper length of direction() */
    constexpr Scalar proper_length() const {
        return _proper_length_factor * _direction.length();
    }
    /** @brief proper length of ray segmment until given position */
    constexpr Scalar proper_length_until(const Scalar t) const {
        return t * _proper_length_factor * _direction.length();
    }

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
    Scalar _proper_length_factor;
};

/**
 * @brief ray interface
 * @note A ray is approximated by straight ray segments.
 */
class Ray {
  public:
    virtual ~Ray() = default;

    /**
     * @brief returns next ray segment or nothing
     * @note A ray may end prematurely due to technical limitations.
     */
    virtual std::optional<RaySegment> next_ray_segment() = 0;
};

} // namespace cpp_raytracing

#endif
