/**
 * @file
 * @brief non-Euclidean twisted orb geometry with Cartesian-like coordinates
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *twisted orb* coordinates:
 *       \code
 *        (u,v,z) = (
 *            sqrt(x^2 + y^2) *
 *                cos(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            sqrt(x^2 + y^2) *
 *                sin(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            z
 *        )
 *       \endcode
 *       For constants `psi` and `rho`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - psi * exp(-sqrt(x^2 + y^2 + z^2) / rho), z)
 *       \endcode
 */

#ifndef CPP_RAYTRACING_GEOMETRY_TWISTED_ORB_CARTESIAN_HPP
#define CPP_RAYTRACING_GEOMETRY_TWISTED_ORB_CARTESIAN_HPP

#include <cmath>
// note specific imports for speed and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include "../algorithm/runge_kutta.hpp"
#include "../values/tensor_boost_numeric_odeint.hpp"
#include "../world/ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

class TwistedOrbCartesianRay;
class TwistedOrbCartesianGeometry;

/**
 * @brief functional object for solving integral of ray propagation
 * @see TwistedOrbCartesianRay, TwistedOrbCartesianGeometry
 */
struct TwistedOrbCartesianRayDifferential {
    /** @brief linked geometry */
    const TwistedOrbCartesianGeometry& geometry;

    /** @brief call */
    void operator()(const Vec6& p, Vec6& dpdt, Scalar t);
};

/**
 * @brief ray for non-Euclidean twisted orb geometry with Cartesian-like
 * coordinates
 * @note vectors are with respect to the tangential space
 * @see TwistedOrbCartesianGeometry
 */
class TwistedOrbCartesianRay : public Ray {
  public:
    /**
     * @brief construct new ray
     * @param geometry linked geometry
     * @param start origin of ray
     * @param direction normalized direction  tangential vector
     */
    TwistedOrbCartesianRay(const TwistedOrbCartesianGeometry& geometry,
                           const Vec3& start, const Vec3& direction);

    virtual ~TwistedOrbCartesianRay() = default;

    virtual std::optional<RaySegment> next_ray_segment() override;

    /** @brief returns current phase */
    const Vec6& phase() const { return _phase; }

  private:
    using State = Vec6;
    using Value = Scalar;
    using Stepper =
        boost::numeric::odeint::runge_kutta_cash_karp54<State, Value>;
    using ControlledStepper =
        typename boost::numeric::odeint::result_of::make_controlled<
            Stepper>::type;
    using System = TwistedOrbCartesianRayDifferential;
    using Iterator =
        boost::numeric::odeint::adaptive_time_iterator<ControlledStepper,
                                                       System, State>;

    static Iterator
    make_phase_iterator(TwistedOrbCartesianRay& ray,
                        const TwistedOrbCartesianGeometry& geometry,
                        const Vec3& start, const Vec3& direction);

  private:
    Vec6 _phase;
    const TwistedOrbCartesianGeometry& _geometry;
    Iterator _phase_iterator;
};

/**
 * @brief non-Euclidean twisted orb geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see TwistedOrbCartesianRay
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *twisted orb* coordinates:
 *       \code
 *        (u,v,z) = (
 *            sqrt(x^2 + y^2) *
 *                cos(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            sqrt(x^2 + y^2) *
 *                sin(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            z
 *        )
 *       \endcode
 *       For constants `psi` and `rho`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - psi * exp(-sqrt(x^2 + y^2 + z^2) / rho), z)
 *       \endcode
 */
class TwistedOrbCartesianGeometry : public Geometry {

    friend class TwistedOrbCartesianRay;

  public:
    /**
     * @brief construct new geometry
     */
    TwistedOrbCartesianGeometry(const Scalar twist_angle,
                                const Scalar twist_radius,
                                const Scalar ray_initial_step_size,
                                const Scalar ray_error_abs,
                                const Scalar ray_error_rel,
                                const Scalar ray_max_length,
                                const Scalar _ray_segment_length_factor);

    virtual ~TwistedOrbCartesianGeometry() = default;

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

    /**
     * @brief returns position in flat space coordinates
     * @note Public access for debugging only.
     * @note The methods to_onb_jacobian() and from_onb_jacobian() are with
     *       respect to this transformation to guarantee interoperability.
     * @see to_onb_jacobian(), from_onb_jacobian(), ray_from()
     */
    Vec3 to_cartesian_coords(const Vec3& position) const;

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
    /** @brief maximal twist angle */
    Scalar _twist_angle;
    /** @brief controlls spatial extend of twisting  */
    Scalar _twist_radius;
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
    const TwistedOrbCartesianRayDifferential _phase_derivative_func;
};

void TwistedOrbCartesianRayDifferential::operator()(const Vec6& p, Vec6& dpdt,
                                                    Scalar t) {
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

TwistedOrbCartesianRay::TwistedOrbCartesianRay(
    const TwistedOrbCartesianGeometry& geometry, const Vec3& start,
    const Vec3& direction)
    : _phase(start, direction),
      _geometry{geometry},
      _phase_iterator{make_phase_iterator(*this, geometry, start, direction)} {}

std::optional<RaySegment> TwistedOrbCartesianRay::next_ray_segment() {

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

    // create next segment
    const Scalar R = position.length();
    const bool outwards = dot(position, velocity) > 0.0;
    if (R / _geometry._twist_radius > 5.0 && outwards) {
        // far away from origin and point away from origin
        // -> conventional infinite ray segment
        return RaySegment{position, velocity, infinity};
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

TwistedOrbCartesianRay::Iterator TwistedOrbCartesianRay::make_phase_iterator(
    TwistedOrbCartesianRay& ray, const TwistedOrbCartesianGeometry& geometry,
    const Vec3& start, const Vec3& direction) {
    using namespace boost::numeric::odeint;

    const Scalar initial_dt = geometry._ray_initial_step_size;
    const Scalar error_abs = geometry._ray_error_abs;
    const Scalar error_rel = geometry._ray_error_abs;

    ControlledStepper stepper = make_controlled(
        error_abs, error_rel, runge_kutta_cash_karp54<State, Value>());

    return make_adaptive_time_iterator_begin<ControlledStepper, System, State>(
        std::move(stepper), geometry._phase_derivative_func, ray._phase, 0.0,
        geometry._ray_max_length, initial_dt);
}

TwistedOrbCartesianGeometry::TwistedOrbCartesianGeometry(
    const Scalar twist_angle, const Scalar twist_radius,
    const Scalar ray_initial_step_size, const Scalar ray_error_abs,
    const Scalar ray_error_rel, const Scalar ray_max_length,
    const Scalar ray_segment_length_factor)
    : _twist_angle(twist_angle),
      _twist_radius(twist_radius),
      _ray_initial_step_size(ray_initial_step_size),
      _ray_error_abs(ray_error_abs),
      _ray_error_rel(ray_error_rel),
      _ray_max_length(ray_max_length),
      _ray_segment_length_factor(ray_segment_length_factor),
      // note: store derivative function once per geometry
      _phase_derivative_func{*this} {}

std::unique_ptr<Ray>
TwistedOrbCartesianGeometry::ray_from(const Vec3& start,
                                      const Vec3& direction) const {
    return std::make_unique<TwistedOrbCartesianRay>(*this, start, direction);
}

std::unique_ptr<Ray>
TwistedOrbCartesianGeometry::ray_passing_through(const Vec3& start,
                                                 const Vec3& target) const {
    // convert positions to flat space
    const Vec3 start_cart = to_cartesian_coords(start);
    const Vec3 target_cart = to_cartesian_coords(target);

    // in flat space direction = difference
    const Vec3 direction_cart = unit_vector(target_cart - start_cart);
    const Mat3x3 jacobian = from_onb_jacobian(start);

    // transform direction back to cuved space
    // note: direction is still normalized after transfomration
    const Vec3 direction = jacobian * direction_cart;

    return std::make_unique<TwistedOrbCartesianRay>(*this, start, direction);
}

Mat3x3
TwistedOrbCartesianGeometry::to_onb_jacobian(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar r = std::sqrt(r2);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar t = std::exp(R / rho);
    const Scalar phi = 1.0 / t * psi + std::atan2(v, u);
    const Scalar cphi = std::cos(phi);
    const Scalar sphi = std::sin(phi);

    return {
        Vec3{
            (u * cphi - r2 * (-v / r2 - u * psi / t / (R * rho)) * sphi) / r,
            (v * cphi - r2 * (u / r2 - v * psi / t / (R * rho)) * sphi) / r,
            (r * z * psi * sphi / t) / (R * rho),
        },
        Vec3{
            (r2 * (-v / r2 - u * psi / t / (R * rho)) * cphi + u * sphi) / r,
            (r2 * (u / r2 - v * psi / t / (R * rho)) * cphi + v * sphi) / r,
            -(r * z * psi * cphi / t) / (R * rho),
        },
        Vec3{0, 0, 1},
    };
}

Mat3x3
TwistedOrbCartesianGeometry::from_onb_jacobian(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar r = std::sqrt(r2);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar t = std::exp(R / rho);
    const Scalar phi = 1.0 / t * psi + std::atan2(v, u);
    const Scalar cphi = std::cos(phi);
    const Scalar sphi = std::sin(phi);

    return {
        Vec3{
            ((u * R * rho - v * r2 * psi / t) * cphi + v * R * rho * sphi) /
                (r * R * rho),
            (-v * R * rho * cphi + (u * R * rho - v * r2 * psi / t) * sphi) /
                (r * R * rho),
            -v * z * psi / t / (R * rho),
        },
        Vec3{
            ((v * R * rho + u * r2 * psi / t) * cphi - u * R * rho * sphi) /
                (r * R * rho),
            (u * R * rho * cphi + (v * R * rho + u * r2 * psi / t) * sphi) /
                (r * R * rho),
            u * z * psi / t / (R * rho),
        },
        Vec3{0, 0, 1},
    };
}

Mat3x3 TwistedOrbCartesianGeometry::metric(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar s = std::exp(-2.0 * R / rho);
    const Scalar t = std::exp(R / rho);

    const Scalar f00 =
        1.0 + s * u * psi * (2.0 * t * v * rho / R + u * r2 * psi / R2) / rho2;
    const Scalar f01 = s * psi *
                       (-t * (u - v) * (u + v) * R * rho + u * v * r2 * psi) /
                       (rho2 * R2);
    const Scalar f02 =
        s * z * psi * (t * v * R * rho + u * r2 * psi) / (R2 * rho2);
    const Scalar f11 =
        1.0 + s * v * psi * (-2.0 * t * u * rho / R + v * r2 * psi / R2) / rho2;
    const Scalar f12 =
        s * z * psi * (-t * u * R * rho + v * r2 * psi) / (R2 * rho2);
    const Scalar f22 = 1.0 + s * r2 * z * z * psi * psi / (R2 * rho2);

    return {
        Vec3{f00, f01, f02},
        Vec3{f01, f11, f12},
        Vec3{f02, f12, f22},
    };
}

Vec3 TwistedOrbCartesianGeometry::normalize(const Vec3& position,
                                            const Vec3& vec) const {
    return vec / std::sqrt(dot(vec, metric(position) * vec));
}

Vec3 TwistedOrbCartesianGeometry::to_cartesian_coords(
    const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar phi = std::atan2(v, u) + psi * std::exp(-R / rho);
    return {r * std::cos(phi), r * std::sin(phi), z};
}

Mat3x3 TwistedOrbCartesianGeometry::inverse_metric(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar s = std::exp(-2.0 * R / rho);
    const Scalar t = std::exp(R / rho);

    const Scalar f00 =
        1.0 + (-2.0 / t * u * v * rho * psi / R + s * v * v * psi * psi) / rho2;
    const Scalar f01 =
        s * psi * (t * (u - v) * (u + v) * rho / R - u * v * psi) / rho2;
    const Scalar f02 = (-1.0 / t) * v * z * psi / (R * rho);
    const Scalar f11 =
        1.0 + (2.0 / t * u * v * rho * psi / R + s * u * u * psi * psi) / rho2;
    const Scalar f12 = (1.0 / t) * u * z * psi / (R * rho);
    const Scalar f22 = 1.0;

    return {
        Vec3{f00, f01, f02},
        Vec3{f01, f11, f12},
        Vec3{f02, f12, f22},
    };
}

Ten3x3x3
TwistedOrbCartesianGeometry::christoffel_1(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar rho3 = rho2 * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar u2 = u * u;
    const Scalar u4 = u2 * u2;
    const Scalar u6 = u4 * u2;
    const Scalar v2 = v * v;
    const Scalar v4 = v2 * v2;
    const Scalar v6 = v4 * v2;
    const Scalar z2 = z * z;
    const Scalar z4 = z2 * z2;
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar R5 = R2 * R2 * R;
    const Scalar t = std::exp(R / rho);
    const Scalar s = std::exp(-2.0 * R / rho);

    // u

    const Scalar fuuu = s * psi *
                        (t * v * R2 * rho * (-u2 * R + (v2 + z2) * rho) -
                         u *
                             (u6 - v2 * (v2 + z2) * R * rho +
                              u2 * (v2 + z2) * (v2 - 2.0 * R * rho) +
                              u4 * (2.0 * v2 + z2 - R * rho)) *
                             psi) /
                        (R5 * rho3);

    const Scalar fuuv =
        -s * u * psi *
        (-t * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
         u * v * (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) *
             psi) /
        (R5 * rho3);

    const Scalar fuuz =
        -s * u * z * psi *
        (t * v * R2 * rho * (R + rho) + u * (u2 + v2) * (R2 + R * rho) * psi) /
        (R5 * rho3);

    const Scalar fuvv =
        s * psi *
        (t * v * R2 * rho * (-v2 * (R - 2.0 * rho) + 3.0 * (u2 + z2) * rho) -
         u *
             (v6 - 2.0 * v2 * z2 * R * rho + u4 * (v2 - R * rho) +
              u2 * (2.0 * v2 + z2) * (v2 - R * rho) + v4 * (z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fuvz = -s * z * psi *
                        (-t * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
                         u * v * (u2 + v2) * (R2 + R * rho) * psi) /
                        (R5 * rho3);

    const Scalar fuzz =
        s * psi *
        (t * v * R2 * rho * (-z2 * R + (u2 + v2) * rho) -
         u *
             (z4 * R * rho + u4 * (z2 - R * rho) + v4 * (z2 - R * rho) +
              v2 * (z4 + z2 * R * rho) +
              u2 * (z4 + z2 * R * rho + 2.0 * v2 * (z2 - R * rho))) *
             psi) /
        (R5 * rho3);

    // v

    const Scalar fvuu =
        s * psi *
        (t * u * R2 * rho * (u2 * (R - 2.0 * rho) - 3.0 * (v2 + z2) * rho) -
         v *
             (u6 - v2 * (v2 + z2) * R * rho +
              u2 * (v2 + z2) * (v2 - 2.0 * R * rho) +
              u4 * (2.0 * v2 + z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fvuv =
        -s * v * psi *
        (-t * R2 * rho * (u2 * R - (v2 + z2) * rho) +
         u * v * (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) *
             psi) /
        (R5 * rho3);

    const Scalar fvuz = -s * z * psi *
                        (-t * R2 * rho * (u2 * R - (v2 + z2) * rho) +
                         u * v * (u2 + v2) * (R2 + R * rho) * psi) /
                        (R5 * rho3);

    const Scalar fvvv =
        -s * psi *
        (t * u * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
         v *
             (v6 - 2.0 * v2 * z2 * R * rho + u4 * (v2 - R * rho) +
              u2 * (2.0 * v2 + z2) * (v2 - R * rho) + v4 * (z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fvvz =
        s * v * z * psi *
        (t * u * R2 * rho * (R + rho) - v * (u2 + v2) * (R2 + R * rho) * psi) /
        (R5 * rho3);

    const Scalar fvzz =
        -s * psi *
        (t * u * R2 * rho * (-z2 * R + (u2 + v2) * rho) +
         v *
             (z4 * R * rho + u4 * (z2 - R * rho) + v4 * (z2 - R * rho) +
              v2 * (z4 + z2 * R * rho) +
              u2 * (z4 + z2 * R * rho + 2.0 * v2 * (z2 - R * rho))) *
             psi) /
        (R5 * rho3);

    // z

    const Scalar fzuu =
        -s * z *
        (u6 - v2 * (v2 + z2) * R * rho + u2 * (v2 + z2) * (v2 - 3.0 * R * rho) +
         u4 * (2.0 * v2 + z2 - 2.0 * R * rho)) *
        psi * psi / (R5 * rho3);

    const Scalar fzuv = -s * u * v * z *
                        (u4 + v4 - 2.0 * z2 * R * rho + v2 * (z2 - R * rho) +
                         u2 * (2.0 * v2 + z2 - R * rho)) *
                        psi * psi / (R5 * rho3);

    const Scalar fzuz =
        -s * u * z2 *
        (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) * psi * psi /
        (R5 * rho3);

    const Scalar fzvv =
        s * z * R *
        (-v4 * (R - 2.0 * rho) + u4 * rho + 3.0 * v2 * z2 * rho +
         u2 * (-v2 * (R - 3.0 * rho) + z2 * rho)) *
        psi * psi / (R5 * rho3);

    const Scalar fzvz =
        -s * v * z2 *
        (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) * psi * psi /
        (R5 * rho3);

    const Scalar fzzz = -s * (u2 + v2) * z *
                        (z4 + u2 * (z2 - R * rho) + v2 * (z2 - R * rho)) * psi *
                        psi / (R5 * rho3);

    return Ten3x3x3{
        Mat3x3{
            Vec3{fuuu, fuuv, fuuz},
            Vec3{fuuv, fuvv, fuvz},
            Vec3{fuuz, fuvz, fuzz},
        },
        Mat3x3{
            Vec3{fvuu, fvuv, fvuz},
            Vec3{fvuv, fvvv, fvvz},
            Vec3{fvuz, fvvz, fvzz},
        },
        Mat3x3{
            Vec3{fzuu, fzuv, fzuz},
            Vec3{fzuv, fzvv, fzvz},
            Vec3{fzuz, fzvz, fzzz},
        },
    };
}

Ten3x3x3
TwistedOrbCartesianGeometry::christoffel_2(const Vec3& position) const {
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
