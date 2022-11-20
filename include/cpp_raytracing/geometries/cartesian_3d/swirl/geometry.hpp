/**
 * @file
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_SWIRL_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_SWIRL_GEOMETRY_HPP

// note specific imports for speed and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include "../../../values/tensor_boost_numeric_odeint.hpp"
#include "../../../world/geometry.hpp"
#include "../../../world/ray_segment.hpp"

namespace cpp_raytracing { namespace cartesian_3d {

class SwirlRay;
class SwirlGeometry;

/**
 * @brief functional object for solving integral of ray propagation
 * @see SwirlRay, SwirlGeometry
 */
struct SwirlRayDifferential {
    /** @brief linked geometry */
    const SwirlGeometry& geometry;

    /** @brief call */
    void operator()(const Vec6& p, Vec6& dpdt, Scalar t);
};

/**
 * @brief ray for non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlGeometry
 */
class SwirlRay : public Ray3D {
  public:
    /**
     * @brief construct new ray
     * @param geometry linked geometry
     * @param start origin of ray
     * @param direction normalized direction  tangential vector
     */
    SwirlRay(const SwirlGeometry& geometry, const Vec3& start,
             const Vec3& direction);

    /** @brief copy constructor */
    SwirlRay(const SwirlRay&) = default;

    /** @brief move constructor */
    SwirlRay(SwirlRay&&) = default;

    /** @brief copy assignment */
    SwirlRay& operator=(const SwirlRay&) = delete;

    /** @brief move assignment */
    SwirlRay& operator=(SwirlRay&&) = delete;

    ~SwirlRay() override = default;

    std::optional<RaySegment3D> next_ray_segment() override;

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
    using System = SwirlRayDifferential;
    using Iterator =
        boost::numeric::odeint::adaptive_time_iterator<ControlledStepper,
                                                       System, State>;

    static Iterator make_phase_iterator(SwirlRay& ray,
                                        const SwirlGeometry& geometry);

  private:
    /** @brief returns current phase (position, velocity)*/
    Vec6 _phase;
    const SwirlGeometry& _geometry;
    Iterator _phase_iterator;
};

/**
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @see SwirlRay
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
class SwirlGeometry : public Geometry3D {

    friend class SwirlRay;

  public:
    /**
     * @brief construct new geometry
     * @note `swirl_strength = 0.0` is equivalent to Euclidean space.
     */
    SwirlGeometry(const Scalar swirl_strength,
                  const Scalar ray_initial_step_size,
                  const Scalar ray_error_abs, const Scalar ray_error_rel,
                  const Scalar ray_max_length,
                  const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    SwirlGeometry(const SwirlGeometry&) = default;

    /** @brief move constructor */
    SwirlGeometry(SwirlGeometry&&) = default;

    /** @brief copy assignment */
    SwirlGeometry& operator=(const SwirlGeometry&) = delete;

    /** @brief move assignment */
    SwirlGeometry& operator=(SwirlGeometry&&) = delete;

    ~SwirlGeometry() override = default;

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray3D> ray_from(const Vec3& start,
                                    const Vec3& direction) const override;

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray3D>
    ray_passing_through(const Vec3& start, const Vec3& target) const override;

    /** @see Geometry::to_onb_jacobian */
    Mat3x3 to_onb_jacobian(const Vec3& position) const override;
    /** @see Geometry::from_onb_jacobian */
    Mat3x3 from_onb_jacobian(const Vec3& position) const override;

    /** @see Geometry::metric */
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
    const SwirlRayDifferential _phase_derivative_func;
};

void SwirlRayDifferential::operator()(const Vec6& p, Vec6& dpdt,
                                      [[maybe_unused]] Scalar t) {
    using namespace tensor;

    const auto [pos, dir] = split(p);
    const Ten3x3x3 chris_2 = geometry.christoffel_2(pos);
    dpdt = outer_sum(dir, gttl::contraction<1, 2>(
                              gttl::contraction<1, 3>(chris_2, dir), -dir));
}

SwirlRay::SwirlRay(const SwirlGeometry& geometry, const Vec3& start,
                   const Vec3& direction)
    : _phase{tensor::outer_sum(start, direction)},
      _geometry{geometry},
      _phase_iterator{make_phase_iterator(*this, geometry)} {}

std::optional<RaySegment3D> SwirlRay::next_ray_segment() {

    using namespace tensor;

    const auto& [phase_start, time] = *_phase_iterator;
    // note: copies! (phase will be updated later)
    const Scalar time_start = time;

    // check for ray length
    if (time_start > _geometry._ray_max_length) {
        // prematurely end ray due to exceeding length
        // note: iterator will stop generating new elements for this time/length
        return std::nullopt;
    }

    const auto [position, velocity] = split(phase_start);
    // check for numerical issues
    if (auto x = length(position), y = length(velocity);
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
    const RaySegment3D segment = {position, velocity, delta_t};

    return segment;
};

SwirlRay::Iterator
SwirlRay::make_phase_iterator(SwirlRay& ray, const SwirlGeometry& geometry) {
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

SwirlGeometry::SwirlGeometry(const Scalar swirl_strength,
                             const Scalar ray_initial_step_size,
                             const Scalar ray_error_abs,
                             const Scalar ray_error_rel,
                             const Scalar ray_max_length,
                             const Scalar ray_segment_length_factor)
    : _swirl_strength(swirl_strength),
      _ray_initial_step_size(ray_initial_step_size),
      _ray_error_abs(ray_error_abs),
      _ray_error_rel(ray_error_rel),
      _ray_max_length(ray_max_length),
      _ray_segment_length_factor(ray_segment_length_factor),
      // note: store derivative function once per geometry
      _phase_derivative_func{*this} {}

std::unique_ptr<Ray3D> SwirlGeometry::ray_from(const Vec3& start,
                                               const Vec3& direction) const {
    return std::make_unique<SwirlRay>(*this, start, direction);
}

std::unique_ptr<Ray3D>
SwirlGeometry::ray_passing_through(const Vec3& start,
                                   const Vec3& target) const {

    const Scalar a = _swirl_strength;

    // start
    const Scalar u0 = start[0];
    const Scalar v0 = start[1];
    const Scalar z0 = start[2];
    const Scalar r0 = std::sqrt(u0 * u0 + v0 * v0);
    const Scalar arz0 = a * r0 * z0;
    const Scalar phi0 = std::atan2(v0, u0) + arz0;

    // target
    const Scalar u1 = target[0];
    const Scalar v1 = target[1];
    const Scalar z1 = target[2];
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

    return std::make_unique<SwirlRay>(*this, start,
                                      normalize(start, direction));
}

Mat3x3 SwirlGeometry::to_onb_jacobian(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    // note: convention is atan2(y, x)
    const Scalar phi = std::atan2(v, u) + a * r * z;
    const Scalar sphi = std::sin(phi);
    const Scalar cphi = std::cos(phi);

    return {
        // clang-format off
        ((u + a * v * z * r) * cphi + v * sphi) / r,
        ((u + a * v * z * r) * sphi - v * cphi) / r,
        a * v * r,

        ((v - a * u * z * r) * cphi - u * sphi) / r,
        ((v - a * u * z * r) * sphi + u * cphi) / r,
        -a * u * r,

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 SwirlGeometry::from_onb_jacobian(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    // note: convention is atan2(y, x)
    const Scalar phi = std::atan2(v, u) + a * r * z;
    const Scalar sphi = std::sin(phi);
    const Scalar cphi = std::cos(phi);

    return {
        // clang-format off
        ((v - a * u * z * r) * sphi + u * cphi) / r,
        (-(u + a * v * z * r) * sphi + v * cphi) / r,
        -a * r * r * sphi,

        ((-v + a * u * z * r) * cphi + u * sphi) / r,
        ((u + a * v * z * r) * cphi + v * sphi) / r,
        a * r * r * cphi,

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 SwirlGeometry::metric(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);

    return {
        // clang-format off
            1 + a * u * z * (a * u * z - 2 * v / r),
            a * z * (u * u - v * v + a * u * v * z * r) / r,
            a * r * (a * u * z * r - v),
   
            (a * z * (u * u - v * v + a * u * v * z * r)) / r,
            1 + a * v * z * (2 * u / r + a * v * z),
            a * (u * r + a * v * z * r * r),

            a * r * (a * u * z * r - v),
            a * r * (u + a * v * z * r),
            1 + a * a * r * r * r * r,
        // clang-format on
    };
}

Vec3 SwirlGeometry::normalize(const Vec3& position, const Vec3& vec) const {
    using namespace tensor;

    return vec * (Scalar{1} / std::sqrt(dot(vec, metric(position) * vec)));
}

Mat3x3 SwirlGeometry::inverse_metric(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    const Scalar s = u * u - v * v;
    const Scalar u2v2z2 = u * u + v * v + z * z;

    return {
        // clang-format off
        1 + a * v * ((2 * u * z) / r + a * v * (r * r + z * z)),
        a * ((-s * z) / r - a * u * v * u2v2z2),
        a * v * r,

        a * ((-s * z) / r - a * u * v * u2v2z2),
        1 + a * u * ((-2 * v * z) / r + a * u * u2v2z2),
        -a * u * r,
      
        a * v * r,
        -a * u * r,
        1,
        // clang-format on
    };
}

Ten3x3x3 SwirlGeometry::christoffel_1(const Vec3& position) const {

    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
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
        // clang-format off
        // x
        a * z * (arz * cos_alpha - sin3_alpha),
        -a * z * cos3_alpha,
        a * r * cos_alpha * (arz * cos_alpha - sin_alpha),

        -a * z * cos3_alpha,
        -0.25 * a * z * (-4 * arz * cos_alpha + 9 * sin_alpha + sin_3alpha),
        0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),

        a * r * cos_alpha * (arz * cos_alpha - sin_alpha),
        0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),
        -a2r3 * cos_alpha,

        // y
        0.25 * a * z * (9 * cos_alpha - cos_3alpha + 4 * arz * sin_alpha),
        a * z * sin3_alpha,
        0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),

        a * z * sin3_alpha,
        a * z * (cos3_alpha + arz * sin_alpha),
        a * r * sin_alpha * (cos_alpha + arz * sin_alpha),

        0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),
        a * r * sin_alpha * (cos_alpha + arz * sin_alpha),
        -a2r3 * sin_alpha,

        // z
        0.5 * a2r2 * z * (3 + cos_2alpha),
        a2r2 * z * cos_alpha * sin_alpha,
        2 * a2r3 * cos_alpha,

        a2r2 * z * cos_alpha * sin_alpha,
        -(0.5) * a2r2 * z * (-3 + cos_2alpha),
        2 * a2r3 * sin_alpha,

        2 * a2r3 * cos_alpha,
        2 * a2r3 * sin_alpha,
        0,
        // clang-format on
    };
}

Ten3x3x3 SwirlGeometry::christoffel_2(const Vec3& position) const {
    const Mat3x3 inv_metric = inverse_metric(position);
    const Ten3x3x3 chris_1 = christoffel_1(position);

    return gttl::contraction<1, 2>(inv_metric, chris_1);
}

}} // namespace cpp_raytracing::cartesian_3d

#endif
