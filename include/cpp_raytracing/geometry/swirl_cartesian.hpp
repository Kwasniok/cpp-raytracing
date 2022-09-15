/**
 * @file
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP
#define CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP

#include "../world/ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief ray for non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlCartesianGeometry
 */
class SwirlCartesianRay : public Ray {
  public:
    /** @brief construct new ray */
    SwirlCartesianRay(const Scalar swirl_strength, const Vec3& start,
                      const Vec3& direction)
        : _start(start),
          _direction(direction),
          _swirl_strength(swirl_strength){};

    virtual ~SwirlCartesianRay() = default;

    virtual std::optional<RaySegment> next_ray_segment() override {
        // TODO: replace dummy with proper implementation
        if (_has_next) {
            _has_next = false;
            return RaySegment{_start, _direction};
        }
        return std::nullopt;
    };

  private:
    Vec3 _start;
    Vec3 _direction;
    Scalar _swirl_strength;
    bool _has_next = true; // TODO: remove
};

/**
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlCartesianRay
 */
class SwirlCartesianGeometry : public Geometry {
  public:
    /**
     * @brief construct new geometry
     * @note `swirl_strength = 0.0` is equivalent to Euclidean space.
     */
    SwirlCartesianGeometry(const Scalar swirl_strength)
        : _swirl_strength(swirl_strength){};

    virtual ~SwirlCartesianGeometry() = default;

    virtual std::unique_ptr<Ray>
    ray_from(const Vec3& start, const Vec3& direction) const override {
        return std::make_unique<SwirlCartesianRay>(_swirl_strength, start,
                                                   direction);
    }

    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3& start, const Vec3& target) const override {

        const Scalar a = _swirl_strength;

        // start
        const Scalar u0 = start.x();
        const Scalar v0 = start.y();
        const Scalar z0 = start.z();
        const Scalar r0 = std::sqrt(u0 * u0 + v0 * v0);
        const Scalar arz0 = a * r0 * z0;
        const Scalar phi0 = std::atan2(v0, u0) + arz0;

        // target
        const Scalar u1 = target.x();
        const Scalar v1 = target.y();
        const Scalar z1 = target.z();
        const Scalar r1 = std::sqrt(u1 * u1 + v1 * v1);
        const Scalar arz1 = a * r1 * z1;
        const Scalar phi1 = std::atan2(v1, u1) + arz1;

        const Vec3 direction = {
            -r0 * std::cos(arz0 - phi0) + r1 * std::cos(arz0 - phi1) -
                a * r0 *
                    (-2 * r0 * z0 + r0 * z1 + r1 * z0 * std::cos(phi1 - phi0)) *
                    std::sin(arz0 - phi0),
            a * r0 * r0 * (2 * z0 - z1) * std::cos(arz0 - phi0) -
                0.5 * arz0 * r1 *
                    (std::cos(arz0 - phi1) + std::cos(arz0 - 2 * phi0 + phi1)) +
                r0 * std::sin(arz0 - phi0) - r1 * std::sin(arz0 - phi1),
            z1 - z0,
        };

        return std::make_unique<SwirlCartesianRay>(a, start, direction);
    }

    virtual Mat3x3 to_onb_jacobian(const Vec3& position) const override {
        const Scalar a = _swirl_strength;
        const Scalar u = position.x();
        const Scalar v = position.y();
        const Scalar z = position.z();
        const Scalar r = std::sqrt(u * u + v * v);
        // note: convention is atan2(y, x)
        const Scalar phi = std::atan2(v, u) + a * r * z;
        const Scalar sphi = std::sin(phi);
        const Scalar cphi = std::cos(phi);

        return {
            Vec3{
                ((u + a * v * z * r) * cphi + v * sphi) / r,
                ((u + a * v * z * r) * sphi - v * cphi) / r,
                a * v * r,
            },
            Vec3{
                ((v - a * u * z * r) * cphi - u * sphi) / r,
                ((v - a * u * z * r) * sphi + u * cphi) / r,
                -a * u * r,
            },
            Vec3{0, 0, 1},
        };
    }
    virtual Mat3x3 from_onb_jacobian(const Vec3& position) const override {
        const Scalar a = _swirl_strength;
        const Scalar u = position.x();
        const Scalar v = position.y();
        const Scalar z = position.z();
        const Scalar r = std::sqrt(u * u + v * v);
        // note: convention is atan2(y, x)
        const Scalar phi = std::atan2(v, u) + a * r * z;
        const Scalar sphi = std::sin(phi);
        const Scalar cphi = std::cos(phi);

        return {
            Vec3{
                ((v - a * u * z * r) * sphi + u * cphi) / r,
                (-(u + a * v * z * r) * sphi + v * cphi) / r,
                -a * r * r * sphi,
            },
            Vec3{
                ((-v + a * u * z * r) * cphi + u * sphi) / r,
                ((u + a * v * z * r) * cphi + v * sphi) / r,
                a * r * r * cphi,
            },
            Vec3{0, 0, 1},
        };
    }

    virtual Mat3x3 metric(const Vec3& position) const override {
        const Scalar a = _swirl_strength;
        const Scalar u = position.x();
        const Scalar v = position.y();
        const Scalar z = position.z();
        const Scalar r = std::sqrt(u * u + v * v);

        return {
            Vec3{
                1 + a * u * z * (a * u * z - 2 * v / r),
                a * z * (u * u - v * v + a * u * v * z * r) / r,
                a * r * (a * u * z * r - v),
            },
            Vec3{
                (a * z * (u * u - v * v + a * u * v * z * r)) / r,
                1 + a * v * z * (2 * u / r + a * v * z),
                a * (u * r + a * v * z * r * r),
            },
            Vec3{
                a * r * (a * u * z * r - v),
                a * r * (u + a * v * z * r),
                1 + a * a * r * r * r * r,
            },
        };
    }

  private:
    /** @brief strength of the swirl effect */
    Scalar _swirl_strength;
};

} // namespace cpp_raytracing

#endif
