/**
 * @file
 * @brief standard Euclidean geometry with Cartesian coordinates where all
 *        coordinates beyond the 3rd are discarded
 * @note To be used for testing and debugging.
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_EMBEDDED_EUCLIDEAN_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_EMBEDDED_EUCLIDEAN_HPP

#include "../../world/geometry.hpp"
#include "../../world/ray_segment.hpp"

namespace cpp_raytracing { namespace cartesian_embedded {

/**
 * @brief Euclidean 3D geometry ray with additional discarded dimensions
 * @note Has only one infinitely long segment.
 * @note To be used for testing and debugging.
 */
template <Dimension DIMENSION>
class EuclideanRay : public Ray<DIMENSION> {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    EuclideanRay(const Vec<DIMENSION>& start, const Vec<DIMENSION>& direction)
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

    std::optional<RaySegment<DIMENSION>> next_ray_segment() override {
        if (_has_next) {
            _has_next = false;
            return RaySegment<DIMENSION>{_start, _direction};
        }
        return std::nullopt;
    };

  private:
    Vec<DIMENSION> _start;
    Vec<DIMENSION> _direction;
    bool _has_next = true;
};

/**
 * @brief Euclidean geometry with Cartesian coordinates
 * @note To be used for testing and debugging.
 */
template <Dimension DIMENSION>
requires(DIMENSION >= 3) class EuclideanGeometry : public Geometry<DIMENSION> {
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
    std::unique_ptr<Ray<DIMENSION>>
    ray_from(const Vec<DIMENSION>& start,
             const Vec<DIMENSION>& direction) const override {
        return std::make_unique<EuclideanRay<DIMENSION>>(start, direction);
    }

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray<DIMENSION>>
    ray_passing_through(const Vec<DIMENSION>& start,
                        const Vec<DIMENSION>& target) const override {
        return std::make_unique<EuclideanRay<DIMENSION>>(
            start, tensor::unit_vector(target - start));
    }

    /** @see Geometry::to_onb_jacobian */
    typename Geometry<DIMENSION>::ToONBJac
    to_onb_jacobian(const Vec<DIMENSION>&) const override {
        using namespace tensor;
        const static Mat<3, DIMENSION> jacobian =
            embeded_matrix<3, DIMENSION, 0, 0, 3, 3>(identity_mat<3_D>);
        return jacobian;
    }
    /** @see Geometry::from_onb_jacobian */
    typename Geometry<DIMENSION>::FromONBJac
    from_onb_jacobian(const Vec<DIMENSION>&) const override {
        using namespace tensor;
        const static Mat<DIMENSION, 3> jacobian =
            embeded_matrix<DIMENSION, 3, 0, 0, 3, 3>(identity_mat<3_D>);
        return jacobian;
    }

    /** @see Geometry::metric */
    typename Geometry<DIMENSION>::Metric
    metric(const Vec<DIMENSION>&) const override {
        return tensor::identity_mat<DIMENSION>;
    }
};

}} // namespace cpp_raytracing::cartesian_embedded

#endif
