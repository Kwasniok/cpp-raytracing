/**
 * @file
 * @brief Cartesian (3+1) Schwarzschild geometry
 * @note The coordinates are (x, y, z, t).
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_GEOMETRY_HPP

#include <cmath>
// note specific imports for speed and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include "../../values/tensor_boost_numeric_odeint.hpp"
#include "../../world/geometries/base.hpp"
#include "../../world/ray_segment.hpp"

namespace cpp_raytracing { namespace schwarzschild {

class SchwarzschildRay;
class SchwarzschildGeometry;

/**
 * @brief functional object for solving integral of ray propagation
 * @see SchwarzschildRay, SchwarzschildGeometry
 */
struct SchwarzschildRayDifferential {
    /** @brief linked geometry */
    const SchwarzschildGeometry& geometry;

    /** @brief call */
    void operator()(const Vec<8_D>& p, Vec<8_D>& dpdt, Scalar t);
};

/**
 * @brief ray for non-Euclidean twisted orb geometry with Cartesian-like
 * coordinates
 * @note vectors are with respect to the tangential space
 * @see SchwarzschildGeometry
 */
class SchwarzschildRay : public Ray<4_D> {
  public:
    /**
     * @brief construct new ray
     * @param geometry linked geometry
     * @param start origin of ray
     * @param direction normalized direction  tangential vector
     */
    SchwarzschildRay(const SchwarzschildGeometry& geometry,
                     const Vec<4_D>& start, const Vec<4_D>& direction);

    /** @brief copy constructor */
    SchwarzschildRay(const SchwarzschildRay&) = default;

    /** @brief move constructor */
    SchwarzschildRay(SchwarzschildRay&&) = default;

    /** @brief copy assignment */
    SchwarzschildRay& operator=(const SchwarzschildRay&) = delete;

    /** @brief move assignment */
    SchwarzschildRay& operator=(SchwarzschildRay&&) = delete;

    ~SchwarzschildRay() override = default;

    std::optional<RaySegment<4_D>> next_ray_segment() override;

    /** @brief returns current phase (position, velocity) */
    const Vec<8_D>& phase() const { return _phase; }

  private:
    using State = Vec<8_D>;
    using Value = Scalar;
    using Stepper =
        boost::numeric::odeint::runge_kutta_cash_karp54<State, Value>;
    using ControlledStepper =
        typename boost::numeric::odeint::result_of::make_controlled<
            Stepper>::type;
    using System = SchwarzschildRayDifferential;
    using Iterator =
        boost::numeric::odeint::adaptive_time_iterator<ControlledStepper,
                                                       System, State>;

    static Iterator make_phase_iterator(SchwarzschildRay& ray,
                                        const SchwarzschildGeometry& geometry);

  private:
    Vec<8_D> _phase;
    const SchwarzschildGeometry& _geometry;
    Iterator _phase_iterator;
};

/**
 * @brief Cartesian (3+1) Schwarzschild geometry
 * @see SchwarzschildRay
 */
class SchwarzschildGeometry : public Geometry<4_D> {

    friend class SchwarzschildRay;

  public:
    /**
     * @brief construct new geometry
     */
    SchwarzschildGeometry(const Scalar twist_angle, const Scalar twist_radius,
                          const Scalar ray_initial_step_size,
                          const Scalar ray_error_abs,
                          const Scalar ray_error_rel,
                          const Scalar ray_max_length,
                          const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    SchwarzschildGeometry(const SchwarzschildGeometry&) = default;

    /** @brief move constructor */
    SchwarzschildGeometry(SchwarzschildGeometry&&) = default;

    /** @brief copy assignment */
    SchwarzschildGeometry& operator=(const SchwarzschildGeometry&) = delete;

    /** @brief move assignment */
    SchwarzschildGeometry& operator=(SchwarzschildGeometry&&) = delete;

    ~SchwarzschildGeometry() override = default;

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray<4_D>>
    ray_from(const Vec<4_D>& start, const Vec<4_D>& direction) const override;

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray<4_D>>
    ray_passing_through(const Vec<4_D>& start,
                        const Vec<4_D>& target) const override;

    /** @see Geometry::to_onb_jacobian */
    Mat<3_D, 4_D> to_onb_jacobian(const Vec<4_D>& position) const override;
    /** @see Geometry::from_onb_jacobian */
    Mat<4_D, 3_D> from_onb_jacobian(const Vec<4_D>& position) const override;

    /** @see Geometry::metric */
    Mat<4_D> metric(const Vec<4_D>& position) const override;

  public:
    /**
     * @brief returns inverse metric
     * @note Public access for debugging only.
     * @see metric()
     */
    Mat<4_D> inverse_metric(const Vec<4_D>& position) const;

    /**
     * @brief returns Chirstoffel symbols of first kind
     * @note Public access for debugging only.
     */
    TenR3<4_D> christoffel_1(const Vec<4_D>& position) const;

    /**
     * @brief returns Chirstoffel symbols of second kind
     * @note Public access for debugging only.
     */
    TenR3<4_D> christoffel_2(const Vec<4_D>& position) const;

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
    const SchwarzschildRayDifferential _phase_derivative_func;
};

void SchwarzschildRayDifferential::operator()(const Vec<8_D>& p, Vec<8_D>& dpdt,
                                              [[maybe_unused]] Scalar t) {
    using namespace tensor;

    const auto [pos, dir] = split(p);
    const auto chris_2 = geometry.christoffel_2(pos);
    dpdt = outer_sum(dir, gttl::contraction<1, 2>(
                              gttl::contraction<1, 3>(chris_2, dir), -dir));
}

SchwarzschildRay::SchwarzschildRay(const SchwarzschildGeometry& geometry,
                                   const Vec<4_D>& start,
                                   const Vec<4_D>& direction)
    : _phase{tensor::outer_sum(start, direction)},
      _geometry{geometry},
      _phase_iterator{make_phase_iterator(*this, geometry)} {}

std::optional<RaySegment<4_D>> SchwarzschildRay::next_ray_segment() {
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
    const RaySegment<4_D> segment = {position, velocity, delta_t};

    return segment;
};

SchwarzschildRay::Iterator
SchwarzschildRay::make_phase_iterator(SchwarzschildRay& ray,
                                      const SchwarzschildGeometry& geometry) {
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

SchwarzschildGeometry::SchwarzschildGeometry(
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

std::unique_ptr<Ray<4_D>>
SchwarzschildGeometry::ray_from(const Vec<4_D>& start,
                                const Vec<4_D>& direction) const {
    return std::make_unique<SchwarzschildRay>(*this, start, direction);
}

std::unique_ptr<Ray<4_D>>
SchwarzschildGeometry::ray_passing_through(const Vec<4_D>& start,
                                           const Vec<4_D>& target) const {
    using namespace tensor;

    // convert positions to flat space
    const Vec<4_D> start_cart = to_cartesian_coords(start);
    const Vec<4_D> target_cart = to_cartesian_coords(target);

    return std::make_unique<SchwarzschildRay>(*this, start, direction);
}

Mat<3_D, 4_D>
SchwarzschildGeometry::to_onb_jacobian(const Vec<4_D>& position) const {}

Mat<4_D, 3_D>
SchwarzschildGeometry::from_onb_jacobian(const Vec<4_D>& position) const {}

Mat<4_D> SchwarzschildGeometry::metric(const Vec<4_D>& position) const {}

Mat<4_D> SchwarzschildGeometry::inverse_metric(const Vec<4_D>& position) const {

}

TenR3<4_D>
SchwarzschildGeometry::christoffel_1(const Vec<4_D>& position) const {}

TenR3<4_D>
SchwarzschildGeometry::christoffel_2(const Vec<4_D>& position) const {
    const Mat<4_D> inv_metric = inverse_metric(position);
    const TenR3<4_D> chris_1 = christoffel_1(position);

    return gttl::contraction<1, 2>(inv_metric, chris_1);
}

}} // namespace cpp_raytracing::schwarzschild

#endif
