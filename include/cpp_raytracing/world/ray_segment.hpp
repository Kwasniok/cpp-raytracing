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
 * @tparam DIMENSION dimension of the vector space a ray propagates in
 */
template <Dimension DIMENSION>
class RaySegment {
  public:
    /** @brief vector type */
    using VecType = Vec<DIMENSION>;

    /**
     * @brief initialize with parameters
     * @param start starting point of ray segment
     * @param direction direction vector must be either zero or finite vector
     * @param t_max end of segment (must be strctly positive and may be
     *        infinity)
     */
    constexpr RaySegment(const VecType& start, const VecType& direction,
                         const Scalar t_max = infinity)
        : _start(start), _direction(direction), _t_max(t_max) {}

    /** @brief starting point of the ray */
    constexpr VecType start() const { return _start; }
    /** @brief direction of the ray */
    constexpr VecType direction() const { return _direction; }
    /** @brief maximal value for parameter t */
    constexpr Scalar t_max() const { return _t_max; }

    /**
     * @brief returns true iff parameter denotes a point within this current
     *        segment
     */
    constexpr bool contains(const Scalar t) const { return t < _t_max; }

    /**
     * @brief returns true iff ray segment is infinite (i.e. `t_max = infinity`)
     * @note Useful for rays consisting of single segment only.
     */
    constexpr bool is_infinite() const { return _t_max == infinity; }

    /**
     * @brief point on the ray for given parameter
     * @return `start() + t * direction()`
     */
    constexpr VecType at(const Scalar t) const {
        return _start + t * _direction;
    }

    /** @brief tests if direction of ray is a zero vector */
    constexpr bool direction_near_zero(const Scalar epsilon) const {
        return tensor::near_zero(_direction, epsilon);
    }

    /** @brief tests if direction of ray is a perfect zero vector */
    constexpr bool direction_exactly_zero() const {
        return _direction[0] == 0.0 && _direction[1] == 0.0 &&
               _direction[2] == 0.0;
    }

  private:
    VecType _start;
    VecType _direction;
    Scalar _t_max;
};

/** @brief 3D ray segment */
using RaySegment3D = RaySegment<Dimension{3}>;

} // namespace cpp_raytracing

#endif
