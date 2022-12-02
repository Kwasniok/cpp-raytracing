/**
 * @file
 * @brief standard Minkowski geometry with Cartesian coordinates (space-time)
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_GEOMETRY_HPP

#include <cmath>

#include "../../world/geometries/base.hpp"
#include "../../world/ray_segment.hpp"

namespace cpp_raytracing { namespace minkowski {
/**
 * @brief ray for Minkowski geometry
 * @note Has only one infinitely long segment.
 */
class Ray : public cpp_raytracing::Ray<4_D> {
  public:
    /**
     * @brief constructs a new infinitely long straight ray
     * @note The temporal direction is implicit.
     * @note Asserts a normalized spatial direction.
     */
    Ray(const Scalar speed_of_light, const Vec<4_D>& start,
        const Vec<3_D>& spatial_direction)
        : _start{start}, _direction{} {
        // note: direction is in future light cone
        _direction[0] = spatial_direction[0];
        _direction[1] = spatial_direction[1];
        _direction[2] = spatial_direction[2];
        _direction[3] = -1.0 / speed_of_light;
    }

    /** @brief copy constructor */
    Ray(const Ray&) = default;

    /** @brief move constructor */
    Ray(Ray&&) = default;

    /** @brief copy assignment */
    Ray& operator=(const Ray&) = default;

    /** @brief move assignment */
    Ray& operator=(Ray&&) = default;

    ~Ray() override = default;

    std::optional<RaySegment<4_D>> next_ray_segment() override {
        if (_has_next) {
            _has_next = false;
            return RaySegment<4_D>{_start, _direction};
        }
        return std::nullopt;
    };

  private:
    Vec<4_D> _start;
    Vec<4_D> _direction;
    bool _has_next = true;
};

/**
 * @brief standard (3+1) Minkowski Cartesian geometry
 */
class Geometry : public cpp_raytracing::Geometry<4_D> {
  public:
    /** @brief conversion factor between spatial and temporal dimensions */
    Scalar speed_of_light;

    /** @brief default constructor */
    Geometry(const Scalar speed_of_light = 1.0)
        : speed_of_light{speed_of_light} {}

    /** @brief copy constructor */
    Geometry(const Geometry&) = default;

    /** @brief move constructor */
    Geometry(Geometry&&) = default;

    /** @brief copy assignment */
    Geometry& operator=(const Geometry&) = default;

    /** @brief move assignment */
    Geometry& operator=(Geometry&&) = default;

    ~Geometry() override = default;

    /**
     * @see cpp_raytracing::Geometry::ray_from
     * @note Ignores time component of direction to ensure light-like
     *       propagation.
     */
    std::unique_ptr<cpp_raytracing::Ray<4_D>>
    ray_from(const Vec<4_D>& start, const Vec<4_D>& direction) const override {
        const Vec<3_D> spatial_direction{
            direction[0],
            direction[1],
            direction[2],
        };
        return std::make_unique<Ray>(speed_of_light, start, spatial_direction);
    }

    /**
     *@see cpp_raytracing::Geometry::ray_passing_through
     * @note Ignores time component of target to ensure light-like
     *       propagation.
     */
    std::unique_ptr<cpp_raytracing::Ray<4_D>>
    ray_passing_through(const Vec<4_D>& start,
                        const Vec<4_D>& target) const override {
        const Vec<3_D> start_spatial{
            start[0],
            start[1],
            start[2],
        };
        const Vec<3_D> target_spatial{
            target[0],
            target[1],
            target[2],
        };
        return std::make_unique<Ray>(
            speed_of_light, start,
            tensor::unit_vector(target_spatial - start_spatial));
    }

    /** @see cpp_raytracing::Geometry::to_onb_jacobian */
    typename cpp_raytracing::Geometry<4_D>::ToONBJac
    to_onb_jacobian(const Vec<4_D>&) const override {
        using namespace tensor;
        const static Mat<3, 4_D> jacobian =
            embeded_matrix<3, 4_D, 0, 0, 3, 3>(identity_mat<3_D>);
        return jacobian;
    }
    /** @see cpp_raytracing::Geometry::from_onb_jacobian */
    typename cpp_raytracing::Geometry<4_D>::FromONBJac
    from_onb_jacobian(const Vec<4_D>&) const override {
        using namespace tensor;
        const static Mat<4_D, 3> jacobian =
            embeded_matrix<4_D, 3, 0, 0, 3, 3>(identity_mat<3_D>);
        return jacobian;
    }

    /** @see cpp_raytracing::Geometry::metric */
    typename cpp_raytracing::Geometry<4_D>::Metric
    metric(const Vec<4_D>&) const override {
        return {
            // clang-format off
            +1, +0, +0, +0,
            +0, +1, +0, +0,
            +0, +0, +1, +0,
            +0, +0, +0, -std::pow(speed_of_light,2),
            // clang-format on
        };
    }
};

}} // namespace cpp_raytracing::minkowski

#endif
