/**
 * @file
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP
#define CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP

#include "../algorithm/runge_kutta.hpp"
#include "../world/ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

class SwirlCartesianRay;
class SwirlCartesianGeometry;

/**
 * @brief ray for non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlCartesianGeometry
 */
class SwirlCartesianRay : public Ray {
  public:
    /**
     * @brief construct new ray
     * @param geometry linked geometry
     * @param start origin of ray
     * @param direction normalized direction  tangential vector
     */
    SwirlCartesianRay(const SwirlCartesianGeometry& geometry, const Vec3& start,
                      const Vec3& direction)
        : _phase(start, direction), _geometry(geometry){};

    virtual ~SwirlCartesianRay() = default;

    virtual std::optional<RaySegment> next_ray_segment() override;

  private:
    /** @brief normalize direction based on position */
    void normalize_phase();

  private:
    /** @note phase = (position, velocity */
    Vec6 _phase;
    const SwirlCartesianGeometry& _geometry;
};

/**
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlCartesianRay
 */
class SwirlCartesianGeometry : public Geometry {

    friend class SwirlCartesianRay;

  public:
    /**
     * @brief construct new geometry
     * @note `swirl_strength = 0.0` is equivalent to Euclidean space.
     */
    SwirlCartesianGeometry(const Scalar swirl_strength,
                           const Scalar ray_step_size)
        : _swirl_strength(swirl_strength), _ray_step_size(ray_step_size){};

    virtual ~SwirlCartesianGeometry() = default;

    virtual std::unique_ptr<Ray> ray_from(const Vec3& start,
                                          const Vec3& direction) const override;

    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3& start, const Vec3& target) const override;

    virtual Mat3x3 to_onb_jacobian(const Vec3& position) const override;
    virtual Mat3x3 from_onb_jacobian(const Vec3& position) const override;

    virtual Mat3x3 metric(const Vec3& position) const override;

  public:
    /** @brief returns normalized vector */
    Vec3 normalize(const Vec3& position, const Vec3& vec) const;

  private:
    /**
     * @brief returns inverse metric
     * @see metric()
     */
    Mat3x3 inverse_metric(const Vec3& position) const;

    /** @brief returns Chirstoffel symbols of first kind */
    Ten3x3x3 christoffel_1(const Vec3& position) const;

    /** @brief returns Chirstoffel symbols of second kind */
    Ten3x3x3 christoffel_2(const Vec3& position) const;

  private:
    /** @brief strength of the swirl effect */
    Scalar _swirl_strength;
    /** @brief ray propagation step size */
    Scalar _ray_step_size;
};

std::optional<RaySegment> SwirlCartesianRay::next_ray_segment() {

    // propagate ray by calculating change of phase
    const std::function<Vec6(const Vec6&)> f = [&geo =
                                                    _geometry](const Vec6& p) {
        const Vec3 pos = p.first_half();
        const Vec3 dir = p.second_half();
        const auto chris_2 = geo.christoffel_2(pos);
        return Vec6{
            p.second_half(),
            Vec3{
                -dot(dir, chris_2[0] * dir),
                -dot(dir, chris_2[1] * dir),
                -dot(dir, chris_2[2] * dir),
            },
        };
    };
    const Vec6 delta_phase =
        runge_kutta_4_delta(f, _phase, _geometry._ray_step_size);

    // create next segment
    // note: direction is approximately constant for small segments
    const Scalar t_max = _geometry._ray_step_size;
    const RaySegment segment = {_phase.first_half(), _phase.second_half(),
                                t_max};

    // update state
    _phase += delta_phase;

    // normalize state
    normalize_phase();

    return segment;
};

void SwirlCartesianRay::normalize_phase() {
    const Vec3 pos = _phase.first_half();
    const Vec3 dir = _phase.second_half();
    _phase = Vec6{pos, _geometry.normalize(pos, dir)};
}

std::unique_ptr<Ray>
SwirlCartesianGeometry::ray_from(const Vec3& start,
                                 const Vec3& direction) const {
    return std::make_unique<SwirlCartesianRay>(*this, start, direction);
}

std::unique_ptr<Ray>
SwirlCartesianGeometry::ray_passing_through(const Vec3& start,
                                            const Vec3& target) const {

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

    Vec3 direction = {
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

    return std::make_unique<SwirlCartesianRay>(*this, start,
                                               normalize(start, direction));
}

Mat3x3 SwirlCartesianGeometry::to_onb_jacobian(const Vec3& position) const {
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

Mat3x3 SwirlCartesianGeometry::from_onb_jacobian(const Vec3& position) const {
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

Mat3x3 SwirlCartesianGeometry::metric(const Vec3& position) const {
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

Vec3 SwirlCartesianGeometry::normalize(const Vec3& position,
                                       const Vec3& vec) const {
    return vec / std::sqrt(dot(vec, metric(position) * vec));
}

Mat3x3 SwirlCartesianGeometry::inverse_metric(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position.x();
    const Scalar v = position.y();
    const Scalar z = position.z();
    const Scalar r = std::sqrt(u * u + v * v);
    const Scalar s = u * u - v * v;
    const Scalar u2v2z2 = u * u + v * v + z * z;

    return {
        Vec3{
            1 + a * v * ((2 * u * z) / r + a * v * (r * r + z * z)),
            a * ((-s * z) / r - a * u * v * u2v2z2),
            a * v * r,
        },
        Vec3{
            a * ((-s * z) / r - a * u * v * u2v2z2),
            1 + a * u * ((-2 * v * z) / r + a * u * u2v2z2),
            -a * u * r,
        },
        Vec3{a * v * r, -a * u * r, 1},
    };
}

Ten3x3x3 SwirlCartesianGeometry::christoffel_1(const Vec3& position) const {

    const Scalar a = _swirl_strength;
    const Scalar u = position.x();
    const Scalar v = position.y();
    const Scalar z = position.z();
    const Scalar r = std::sqrt(u * u + v * v);

    const Scalar arz = a * r * z;
    const Scalar a2r2 = a * a * r * r;
    const Scalar a2r3 = a * a * r * r * r;

    const Scalar alpha = std::atan2(v, u);

    // trigonometric
    const Scalar cos_alpha = std::cos(alpha);
    const Scalar sin_alpha = std::sin(alpha);
    const Scalar cos_2alpha = std::cos(2 * alpha);
    const Scalar sin_2alpha = std::sin(2 * alpha);
    const Scalar cos_3alpha = std::cos(3 * alpha);
    const Scalar sin_3alpha = std::sin(3 * alpha);

    // trigonometric powers
    const Scalar cos3_alpha = std::pow(cos_alpha, 3);
    const Scalar sin3_alpha = std::pow(sin_alpha, 3);

    return {
        Mat3x3{
            Vec3{
                a * z * (arz * cos_alpha - sin3_alpha),
                -a * z * cos3_alpha,
                a * r * cos_alpha * (arz * cos_alpha - sin_alpha),
            },
            Vec3{
                -a * z * cos3_alpha,
                -0.25 * a * z *
                    (-4 * arz * cos_alpha + 9 * sin_alpha + sin_3alpha),
                0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),
            },
            Vec3{a * r * cos_alpha * (arz * cos_alpha - sin_alpha),
                 0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),
                 -a2r3 * cos_alpha},
        },
        Mat3x3{
            Vec3{
                0.25 * a * z *
                    (9 * cos_alpha - cos_3alpha + 4 * arz * sin_alpha),
                a * z * sin3_alpha,
                0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),
            },
            Vec3{
                a * z * sin3_alpha,
                a * z * (cos3_alpha + arz * sin_alpha),
                a * r * sin_alpha * (cos_alpha + arz * sin_alpha),
            },
            Vec3{
                0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),
                a * r * sin_alpha * (cos_alpha + arz * sin_alpha),
                -a2r3 * sin_alpha,
            },
        },
        Mat3x3{
            Vec3{
                0.5 * a2r2 * z * (3 + cos_2alpha),
                a2r2 * z * cos_alpha * sin_alpha,
                2 * a2r3 * cos_alpha,
            },
            Vec3{
                a2r2 * z * cos_alpha * sin_alpha,
                -(0.5) * a2r2 * z * (-3 + cos_2alpha),
                2 * a2r3 * sin_alpha,
            },
            Vec3{
                2 * a2r3 * cos_alpha,
                2 * a2r3 * sin_alpha,
                0,
            },
        },
    };
}

Ten3x3x3 SwirlCartesianGeometry::christoffel_2(const Vec3& position) const {
    const Mat3x3 inv_metric = inverse_metric(position);
    const Ten3x3x3 chris_1 = christoffel_1(position);

    return {
        chris_1[0] * inv_metric[0][0] + chris_1[1] * inv_metric[0][1] +
            chris_1[2] * inv_metric[0][2],
        chris_1[0] * inv_metric[1][0] + chris_1[1] * inv_metric[1][1] +
            chris_1[2] * inv_metric[1][2],
        chris_1[0] * inv_metric[2][0] + chris_1[1] * inv_metric[2][1] +
            chris_1[2] * inv_metric[2][2],
    };
}

} // namespace cpp_raytracing

#endif
