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
 * @brief euclidean geometry ray
 * @note Has only one infinitely long segment.
 */
class EuclideanRay : public Ray {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    EuclideanRay(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction){};

    virtual ~EuclideanRay() = default;

    virtual std::optional<RaySegment> next_ray_segment() override {
        if (_has_next) {
            _has_next = false;
            return RaySegment{_start, _direction};
        }
        return std::nullopt;
    };

  private:
    Vec3 _start;
    Vec3 _direction;
    bool _has_next = true;
};

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
