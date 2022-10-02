/**
 * @file
 * @brief standard Euclidean geometry with Cartesian coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRY_EUCLIDEAN_HPP
#define CPP_RAYTRACING_GEOMETRY_EUCLIDEAN_HPP

#include "../world/ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief Euclidean geometry ray
 * @note Has only one infinitely long segment.
 */
class EuclideanRay : public Ray {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    EuclideanRay(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction){};

    /** @brief copy constructor */
    EuclideanRay(const EuclideanRay&) = default;

    /** @brief move constructor */
    EuclideanRay(EuclideanRay&&) = default;

    /** @brief copy assignment */
    EuclideanRay& operator=(const EuclideanRay&) = default;

    /** @brief move assignment */
    EuclideanRay& operator=(EuclideanRay&&) = default;

    ~EuclideanRay() override = default;

    std::optional<RaySegment> next_ray_segment() override {
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
 * @brief Euclidean geometry with Cartesian coordinates
 */
class EuclideanGeometry : public Geometry {
  public:
    /** @brief default constructor */
    EuclideanGeometry() = default;

    /** @brief copy constructor */
    EuclideanGeometry(const EuclideanGeometry&) = default;

    /** @brief move constructor */
    EuclideanGeometry(EuclideanGeometry&&) = default;

    /** @brief copy assignment */
    EuclideanGeometry& operator=(const EuclideanGeometry&) = default;

    /** @brief move assignment */
    EuclideanGeometry& operator=(EuclideanGeometry&&) = default;

    ~EuclideanGeometry() override = default;

    std::unique_ptr<Ray> ray_from(const Vec3& start,
                                  const Vec3& direction) const override {
        return std::make_unique<EuclideanRay>(start, direction);
    }

    std::unique_ptr<Ray>
    ray_passing_through(const Vec3& start, const Vec3& target) const override {
        return std::make_unique<EuclideanRay>(start,
                                              unit_vector(target - start));
    }

    Mat3x3 to_onb_jacobian(const Vec3&) const override {
        return Mat3x3::identity();
    }
    Mat3x3 from_onb_jacobian(const Vec3&) const override {
        return Mat3x3::identity();
    }

    Mat3x3 metric(const Vec3&) const override { return Mat3x3::identity(); }
};

} // namespace cpp_raytracing

#endif
