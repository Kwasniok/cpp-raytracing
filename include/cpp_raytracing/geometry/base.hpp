/**
 * @file
 * @brief geometry interface
 */

#ifndef CPP_RAYTRACING_GEOMETRY_BASE_HPP
#define CPP_RAYTRACING_GEOMETRY_BASE_HPP

#include <memory>

#include "../values/tensor.hpp"
#include "../world/ray.hpp"

namespace cpp_raytracing {

/**
 * @brief geometry interface
 */
class Geometry {
  public:
    virtual ~Geometry() = default;

    /** @brief returns a ray with given starting point and direction */
    virtual std::unique_ptr<Ray> ray_from(const Vec3 start,
                                          const Vec3 direction) const = 0;

    /** @brief returns a ray with given starting point and target */
    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3 start, const Vec3 target) const = 0;

    /** @brief returns the local metric */
    virtual Mat3x3 metric(const Vec3 position) const = 0;
};

} // namespace cpp_raytracing

#endif
