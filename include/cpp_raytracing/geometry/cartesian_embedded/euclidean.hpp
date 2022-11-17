/**
 * @file
 * @brief standard Euclidean geometry with Cartesian coordinates where all
 *        coordinates beyond the 3rd are discarded
 * @note To be used for testing and debugging.
 */

#ifndef CPP_RAYTRACING_GEOMETRY_CARTESIAN_EMBEDDED_EUCLIDEAN_HPP
#define CPP_RAYTRACING_GEOMETRY_CARTESIAN_EMBEDDED_EUCLIDEAN_HPP

#include "../../world/ray_segment.hpp"
#include "../base.hpp"

namespace cpp_raytracing {

/**
 * @brief Euclidean 3D geometry ray with additional discarded dimensions
 * @note Has only one infinitely long segment.
 * @note To be used for testing and debugging.
 */
template <Dimension DIMENSION>
class CartesianEmbeddedEuclideanRay : public Ray<DIMENSION> {
  public:
    /** @brief constructs a new infinitely long straight ray  */
    CartesianEmbeddedEuclideanRay(const Vec<DIMENSION>& start,
                                  const Vec<DIMENSION>& direction)
        : _start(start), _direction(direction){};

    /** @brief copy constructor */
    CartesianEmbeddedEuclideanRay(const CartesianEmbeddedEuclideanRay&) =
        default;

    /** @brief move constructor */
    CartesianEmbeddedEuclideanRay(CartesianEmbeddedEuclideanRay&&) = default;

    /** @brief copy assignment */
    CartesianEmbeddedEuclideanRay&
    operator=(const CartesianEmbeddedEuclideanRay&) = default;

    /** @brief move assignment */
    CartesianEmbeddedEuclideanRay&
    operator=(CartesianEmbeddedEuclideanRay&&) = default;

    ~CartesianEmbeddedEuclideanRay() override = default;

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
requires(DIMENSION > 3) class CartesianEmbeddedEuclideanGeometry
    : public Geometry<DIMENSION> {
  public:
    /** @brief default constructor */
    CartesianEmbeddedEuclideanGeometry() = default;

    /** @brief copy constructor */
    CartesianEmbeddedEuclideanGeometry(
        const CartesianEmbeddedEuclideanGeometry&) = default;

    /** @brief move constructor */
    CartesianEmbeddedEuclideanGeometry(CartesianEmbeddedEuclideanGeometry&&) =
        default;

    /** @brief copy assignment */
    CartesianEmbeddedEuclideanGeometry&
    operator=(const CartesianEmbeddedEuclideanGeometry&) = default;

    /** @brief move assignment */
    CartesianEmbeddedEuclideanGeometry&
    operator=(CartesianEmbeddedEuclideanGeometry&&) = default;

    ~CartesianEmbeddedEuclideanGeometry() override = default;

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray<DIMENSION>>
    ray_from(const Vec<DIMENSION>& start,
             const Vec<DIMENSION>& direction) const override {
        return std::make_unique<CartesianEmbeddedEuclideanRay<DIMENSION>>(
            start, direction);
    }

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray<DIMENSION>>
    ray_passing_through(const Vec<DIMENSION>& start,
                        const Vec<DIMENSION>& target) const override {
        return std::make_unique<CartesianEmbeddedEuclideanRay<DIMENSION>>(
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

} // namespace cpp_raytracing

#endif
