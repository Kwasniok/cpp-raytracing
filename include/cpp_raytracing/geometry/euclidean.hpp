/**
 * @file
 * @brief euclidean geometry
 */

#ifndef CPP_RAYTRACING_GEOMETRY_EUCLIDEAN_HPP
#define CPP_RAYTRACING_GEOMETRY_EUCLIDEAN_HPP

#include "../world/ray.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief geometry interface
 */
class EuclideanGeometry : public Geometry {
  public:
    virtual ~EuclideanGeometry() = default;

    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3 start, const Vec3 target) override {
        return std::make_unique<EuclideanRay>(start,
                                              unit_vector(target - start));
    }

    virtual Mat3x3 metric(const Vec3 position) override {
        return Mat3x3::identity();
    }
};

} // namespace cpp_raytracing

#endif
