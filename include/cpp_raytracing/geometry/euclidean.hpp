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

    virtual std::unique_ptr<Ray> ray_from(const Vec3 start,
                                          const Vec3 direction) const override {
        return std::make_unique<EuclideanRay>(start, direction);
    }

    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3 start, const Vec3 target) const override {
        return std::make_unique<EuclideanRay>(start,
                                              unit_vector(target - start));
    }

    virtual Mat3x3 to_onb_jacobian(const Vec3 position) const override {
        return Mat3x3::identity();
    }
    virtual Mat3x3 from_onb_jacobian(const Vec3 position) const override {
        return Mat3x3::identity();
    }

    virtual Mat3x3 metric(const Vec3 position) const override {
        return Mat3x3::identity();
    }
};

} // namespace cpp_raytracing

#endif
