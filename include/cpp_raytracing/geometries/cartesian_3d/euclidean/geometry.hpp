/**
 * @file
 * @brief standard Euclidean geometry with Cartesian coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_EUCLIDEAN_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_EUCLIDEAN_GEOMETRY_HPP

#include "../../../world/geometries/base.hpp"
#include "../../../world/ray_segment.hpp"

namespace cpp_raytracing { namespace cartesian_3d {

/**
 * @brief Euclidean geometry ray
 * @note Has only one infinitely long segment.
 */
class EuclideanRay : public Ray3D {
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
class EuclideanGeometry : public Geometry3D {
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

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray3D> ray_from(const Vec3& start,
                                    const Vec3& direction) const override {
        return std::make_unique<EuclideanRay>(start, direction);
    }

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray3D>
    ray_passing_through(const Vec3& start, const Vec3& target) const override {
        return std::make_unique<EuclideanRay>(
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

}} // namespace cpp_raytracing::cartesian_3d

#endif
