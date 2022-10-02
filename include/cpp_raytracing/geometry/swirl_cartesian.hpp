/**
 * @file
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP
#define CPP_RAYTRACING_GEOMETRY_SWIRL_CARTESIAN_HPP

// note specific imports for speed and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include "../values/tensor_boost_numeric_odeint.hpp"
#include "../world/ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

class SwirlCartesianRay;
class SwirlCartesianGeometry;

/**
 * @brief functional object for solving integral of ray propagation
 * @see SwirlCartesianRay, SwirlCartesianGeometry
 */
struct SwirlCartesianRayDifferential {
    /** @brief linked geometry */
    const SwirlCartesianGeometry& geometry;

    /** @brief call */
    void operator()(const Vec6& p, Vec6& dpdt, Scalar t);
};

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
                      const Vec3& direction);

    /** @brief copy constructor */
    SwirlCartesianRay(const SwirlCartesianRay&) = default;

    /** @brief move constructor */
    SwirlCartesianRay(SwirlCartesianRay&&) = default;

    /** @brief copy assignment */
    SwirlCartesianRay& operator=(const SwirlCartesianRay&) = delete;

    /** @brief move assignment */
    SwirlCartesianRay& operator=(SwirlCartesianRay&&) = delete;

    ~SwirlCartesianRay() override = default;

    std::optional<RaySegment> next_ray_segment() override;

  private:
    /** @brief returns current phase (position, velocity) */
    const Vec6& phase() const { return _phase; }

  private:
    using State = Vec6;
    using Value = Scalar;
    using Stepper =
        boost::numeric::odeint::runge_kutta_cash_karp54<State, Value>;
    using ControlledStepper =
        typename boost::numeric::odeint::result_of::make_controlled<
            Stepper>::type;
    using System = SwirlCartesianRayDifferential;
    using Iterator =
        boost::numeric::odeint::adaptive_time_iterator<ControlledStepper,
                                                       System, State>;

    static Iterator make_phase_iterator(SwirlCartesianRay& ray,
                                        const SwirlCartesianGeometry& geometry);

  private:
    /** @brief returns current phase (position, velocity)*/
    Vec6 _phase;
    const SwirlCartesianGeometry& _geometry;
    Iterator _phase_iterator;
};

/**
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlCartesianRay
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *swirl* coordinates:
 *       \code
 *        (u,v,z) = (
 *            r * cos(phi - a * r * z),
 *            r * sin(phi - a * r * z),
 *            z
 *        )
 *       where r = sqrt(x * x + y * y), phi = arctan(y / x)
 *       \endcode
 *       For constant `a`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - a * r * z, z)
 *       \endcode
 */
class SwirlCartesianGeometry : public Geometry {

    friend class SwirlCartesianRay;

  public:
    /**
     * @brief construct new geometry
     * @note `swirl_strength = 0.0` is equivalent to Euclidean space.
     */
    SwirlCartesianGeometry(const Scalar swirl_strength,
                           const Scalar ray_initial_step_size,
                           const Scalar ray_error_abs,
                           const Scalar ray_error_rel,
                           const Scalar ray_max_length,
                           const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    SwirlCartesianGeometry(const SwirlCartesianGeometry&) = default;

    /** @brief move constructor */
    SwirlCartesianGeometry(SwirlCartesianGeometry&&) = default;

    /** @brief copy assignment */
    SwirlCartesianGeometry& operator=(const SwirlCartesianGeometry&) = delete;

    /** @brief move assignment */
    SwirlCartesianGeometry& operator=(SwirlCartesianGeometry&&) = delete;

    ~SwirlCartesianGeometry() override = default;

    std::unique_ptr<Ray> ray_from(const Vec3& start,
                                  const Vec3& direction) const override;

    std::unique_ptr<Ray> ray_passing_through(const Vec3& start,
                                             const Vec3& target) const override;

    Mat3x3 to_onb_jacobian(const Vec3& position) const override;
    Mat3x3 from_onb_jacobian(const Vec3& position) const override;

    Mat3x3 metric(const Vec3& position) const override;

  public:
    /** @brief returns normalized vector */
    Vec3 normalize(const Vec3& position, const Vec3& vec) const;

    /**
     * @brief returns inverse metric
     * @note Public access for debugging only.
     * @see metric()
     */
    Mat3x3 inverse_metric(const Vec3& position) const;
    /**
     * @brief returns Chirstoffel symbols of first kind
     * @note Public access for debugging only.
     */
    Ten3x3x3 christoffel_1(const Vec3& position) const;

    /**
     * @brief returns Chirstoffel symbols of second kind
     * @note Public access for debugging only.
     */
    Ten3x3x3 christoffel_2(const Vec3& position) const;

  private:
    /** @brief strength of the swirl effect */
    Scalar _swirl_strength;
    /** @brief initial step size for rays */
    Scalar _ray_initial_step_size;
    /** @brief absolute error tolerance for rays */
    Scalar _ray_error_abs;
    /** @brief relative error tolerance for rays */
    Scalar _ray_error_rel;
    /** @brief upper limit for ray length */
    Scalar _ray_max_length;
    /**
     * @brief factor by which to stretch each ray segment
     * @note Should be a bit larger than `1.0` to avoid small scale geometrical
     *        banding.
     */
    Scalar _ray_segment_length_factor;

    /** @brief differential equation for phase velocity */
    const SwirlCartesianRayDifferential _phase_derivative_func;
};

void SwirlCartesianRayDifferential::operator()(const Vec6& p, Vec6& dpdt,
                                               [[maybe_unused]] Scalar t) {
    const Vec3 pos = p.first_half();
    const Vec3 dir = p.second_half();
    const Ten3x3x3 chris_2 = geometry.christoffel_2(pos);
    dpdt = {
        p.second_half(),
        Vec3{
            -dot(dir, chris_2[0] * dir),
            -dot(dir, chris_2[1] * dir),
            -dot(dir, chris_2[2] * dir),
        },
    };
}

SwirlCartesianRay::SwirlCartesianRay(const SwirlCartesianGeometry& geometry,
                                     const Vec3& start, const Vec3& direction)
    : _phase(start, direction),
      _geometry{geometry},
      _phase_iterator{make_phase_iterator(*this, geometry)} {}

std::optional<RaySegment> SwirlCartesianRay::next_ray_segment() {

    const auto& [phase_start, time] = *_phase_iterator;
    // note: copies! (phase will be updated later)
    const Scalar time_start = time;

    // check for ray length
    if (time_start > _geometry._ray_max_length) {
        // prematurely end ray due to exceeding length
        // note: iterator will stop generating new elements for this time/length
        return std::nullopt;
    }

    const Vec3 position = phase_start.first_half();
    const Vec3 velocity = phase_start.second_half();

    // check for numerical issues
    if (auto x = position.length(), y = velocity.length();
        !(0.0 < x && x < infinity) || !(0.0 < y && y < infinity)) {
        // encountered a numerical issue -> abort ray
        return std::nullopt;
    }

    // update state
    ++_phase_iterator;

    // calculate segment
    // determine dt
    const auto& [_, time_end] = *_phase_iterator;
    // note: extend interval a tiny bit to avoid small scale geometrical banding
    const Scalar delta_t =
        (time_end - time_start) * _geometry._ray_segment_length_factor;
    // note: direction is approximately constant for small segments
    // note: use initial position and velocity
    const RaySegment segment = {position, velocity, delta_t};

    return segment;
};

SwirlCartesianRay::Iterator
SwirlCartesianRay::make_phase_iterator(SwirlCartesianRay& ray,
                                       const SwirlCartesianGeometry& geometry) {
    using namespace boost::numeric::odeint;

    const Scalar initial_dt = geometry._ray_initial_step_size;
    const Scalar error_abs = geometry._ray_error_abs;
    const Scalar error_rel = geometry._ray_error_rel;

    ControlledStepper stepper = make_controlled(
        error_abs, error_rel, runge_kutta_cash_karp54<State, Value>());

    return make_adaptive_time_iterator_begin<ControlledStepper, System, State>(
        std::move(stepper), geometry._phase_derivative_func, ray._phase, 0.0,
        geometry._ray_max_length, initial_dt);
}

SwirlCartesianGeometry::SwirlCartesianGeometry(
    const Scalar swirl_strength, const Scalar ray_initial_step_size,
    const Scalar ray_error_abs, const Scalar ray_error_rel,
    const Scalar ray_max_length, const Scalar ray_segment_length_factor)
    : _swirl_strength(swirl_strength),
      _ray_initial_step_size(ray_initial_step_size),
      _ray_error_abs(ray_error_abs),
      _ray_error_rel(ray_error_rel),
      _ray_max_length(ray_max_length),
      _ray_segment_length_factor(ray_segment_length_factor),
      // note: store derivative function once per geometry
      _phase_derivative_func{*this} {}

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
