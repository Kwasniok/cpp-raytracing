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
class EuclideanRay3D : public Ray3D {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    EuclideanRay3D(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction){};

    /** @brief copy constructor */
    EuclideanRay3D(const EuclideanRay3D&) = default;

    /** @brief move constructor */
    EuclideanRay3D(EuclideanRay3D&&) = default;

    /** @brief copy assignment */
    EuclideanRay3D& operator=(const EuclideanRay3D&) = default;

    /** @brief move assignment */
    EuclideanRay3D& operator=(EuclideanRay3D&&) = default;

    ~EuclideanRay3D() override = default;

    std::optional<RaySegment3D> next_ray_segment() override {
        if (_has_next) {
            _has_next = false;
            return RaySegment3D{_start, _direction};
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
class EuclideanGeometry3D : public Geometry3D {
  public:
    /** @brief default constructor */
    EuclideanGeometry3D() = default;

    /** @brief copy constructor */
    EuclideanGeometry3D(const EuclideanGeometry3D&) = default;

    /** @brief move constructor */
    EuclideanGeometry3D(EuclideanGeometry3D&&) = default;

    /** @brief copy assignment */
    EuclideanGeometry3D& operator=(const EuclideanGeometry3D&) = default;

    /** @brief move assignment */
    EuclideanGeometry3D& operator=(EuclideanGeometry3D&&) = default;

    ~EuclideanGeometry3D() override = default;

    std::unique_ptr<Ray3D> ray_from(const Vec3& start,
                                    const Vec3& direction) const override {
        return std::make_unique<EuclideanRay3D>(start, direction);
    }

    std::unique_ptr<Ray3D>
    ray_passing_through(const Vec3& start, const Vec3& target) const override {
        return std::make_unique<EuclideanRay3D>(
            start, tensor::unit_vector(target - start));
    }

    Mat3x3 to_onb_jacobian(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }
    Mat3x3 from_onb_jacobian(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }

    Mat3x3 metric(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }
};

} // namespace cpp_raytracing

#endif
