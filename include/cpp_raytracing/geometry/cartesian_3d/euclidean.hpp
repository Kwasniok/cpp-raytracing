/**
 * @file
 * @brief standard Euclidean geometry with Cartesian coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRY_CARTESIAN_3D_EUCLIDEAN_HPP
#define CPP_RAYTRACING_GEOMETRY_CARTESIAN_3D_EUCLIDEAN_HPP

#include "../../world/ray_segment.hpp"
#include "../base.hpp"

namespace cpp_raytracing {

/**
 * @brief Euclidean geometry ray
 * @note Has only one infinitely long segment.
 */
class EuclideanCartesianRay3D : public Ray3D {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    EuclideanCartesianRay3D(const Vec3& start, const Vec3& direction)
        : _start(start), _direction(direction){};

    /** @brief copy constructor */
    EuclideanCartesianRay3D(const EuclideanCartesianRay3D&) = default;

    /** @brief move constructor */
    EuclideanCartesianRay3D(EuclideanCartesianRay3D&&) = default;

    /** @brief copy assignment */
    EuclideanCartesianRay3D&
    operator=(const EuclideanCartesianRay3D&) = default;

    /** @brief move assignment */
    EuclideanCartesianRay3D& operator=(EuclideanCartesianRay3D&&) = default;

    ~EuclideanCartesianRay3D() override = default;

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
class EuclideanCartesianGeometry3D : public Geometry3D {
  public:
    /** @brief default constructor */
    EuclideanCartesianGeometry3D() = default;

    /** @brief copy constructor */
    EuclideanCartesianGeometry3D(const EuclideanCartesianGeometry3D&) = default;

    /** @brief move constructor */
    EuclideanCartesianGeometry3D(EuclideanCartesianGeometry3D&&) = default;

    /** @brief copy assignment */
    EuclideanCartesianGeometry3D&
    operator=(const EuclideanCartesianGeometry3D&) = default;

    /** @brief move assignment */
    EuclideanCartesianGeometry3D&
    operator=(EuclideanCartesianGeometry3D&&) = default;

    ~EuclideanCartesianGeometry3D() override = default;

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray3D> ray_from(const Vec3& start,
                                    const Vec3& direction) const override {
        return std::make_unique<EuclideanCartesianRay3D>(start, direction);
    }

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray3D>
    ray_passing_through(const Vec3& start, const Vec3& target) const override {
        return std::make_unique<EuclideanCartesianRay3D>(
            start, tensor::unit_vector(target - start));
    }

    /** @see Geometry::to_onb_jacobian */
    Mat3x3 to_onb_jacobian(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }
    /** @see Geometry::from_onb_jacobian */
    Mat3x3 from_onb_jacobian(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }

    /** @see Geometry::metric */
    Mat3x3 metric(const Vec3&) const override {
        return tensor::identity_mat<3_D>;
    }
};

} // namespace cpp_raytracing

#endif
