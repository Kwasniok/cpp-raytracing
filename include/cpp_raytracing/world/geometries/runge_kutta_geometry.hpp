/**
 * @file
 * @brief geometries with rays calculated via a Runge-Kutta integrator
 */

#ifndef CPP_RAYTRACING_WORLD_GEOMETRIES_RUNGE_KUTTA_GEOMETRY_HPP
#define CPP_RAYTRACING_WORLD_GEOMETRIES_RUNGE_KUTTA_GEOMETRY_HPP

#include <cmath>
#include <concepts>

// note specific imports for speed and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include "../../values/tensor_boost_numeric_odeint.hpp"
#include "../ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

template <Dimension DIMENSION>
class RungeKuttaRay;

template <Dimension DIMENSION>
class RungeKuttaGeometry;

/**
 * @brief ray differential closure
 */
template <Dimension DIMENSION>
struct RungeKuttaRayDifferential {

    /** @brief linked geometry */
    const RungeKuttaGeometry<DIMENSION>& geometry;

    /**
     * @brief phase differential function
     * @note  boost::numeric::odeint reqquires this shape
     */
    void operator()(const Vec<2 * DIMENSION>& phase,
                    Vec<2 * DIMENSION>& dphase_dtime, const Scalar time);
};

/**
 * @brief ray for RungeKuttaGeometry
 * @note vectors are with respect to the tangential space
 * @see RungeKuttaGeometry
 */
template <Dimension DIMENSION>
class RungeKuttaRay : public Ray<DIMENSION> {

  private:
    using State = Vec<2 * DIMENSION>;
    using Value = Scalar;
    using Stepper =
        boost::numeric::odeint::runge_kutta_cash_karp54<State, Value>;
    using ControlledStepper =
        typename boost::numeric::odeint::result_of::make_controlled<
            Stepper>::type;
    using System = RungeKuttaRayDifferential<DIMENSION>;
    using Iterator =
        boost::numeric::odeint::adaptive_time_iterator<ControlledStepper,
                                                       System, State>;

  public:
    /**
     * @brief construct new ray
     * @param geometry linked geometry
     * @param start origin of ray
     * @param direction normalized direction  tangential vector
     * @see RungeKuttaGeometry
     */
    RungeKuttaRay(const RungeKuttaGeometry<DIMENSION>& geometry,
                  const Vec<DIMENSION>& start, const Vec<DIMENSION>& direction);

    /** @brief copy constructor */
    RungeKuttaRay(const RungeKuttaRay&) = default;

    /** @brief move constructor */
    RungeKuttaRay(RungeKuttaRay&&) = default;

    /** @brief copy assignment */
    RungeKuttaRay& operator=(const RungeKuttaRay&) = delete;

    /** @brief move assignment */
    RungeKuttaRay& operator=(RungeKuttaRay&&) = delete;

    ~RungeKuttaRay() override = default;

    std::optional<RaySegment<DIMENSION>> next_ray_segment() override;

    /** @brief returns current phase (position, velocity) */
    const Vec<2 * DIMENSION>& phase() const { return _phase; }
    /** @brief access to phase iterator */
    Iterator& phase_iterator() const { return _phase_iterator; }

  private:
    static Iterator
    make_phase_iterator(const RungeKuttaGeometry<DIMENSION>& geometry,
                        RungeKuttaRay& ray);

  private:
    Vec<2 * DIMENSION> _phase;
    Iterator _phase_iterator;
    const RungeKuttaGeometry<DIMENSION>& _geometry;
};

/**
 * @brief geometry where each ray is approximated by straight segments
 *        calculated via a Runge-Kutta integrator
 * @see RungeKuttaRay
 */
template <Dimension DIMENSION>
class RungeKuttaGeometry : public Geometry<DIMENSION> {

  public:
    /** @brief dmiension of the geometry */
    constexpr static Dimension dimension = DIMENSION;

  public:
    /**
     * @brief construct new geometry
     */
    RungeKuttaGeometry(const Scalar ray_initial_step_size,
                       const Scalar ray_error_abs, const Scalar ray_error_rel,
                       const Scalar ray_max_length,
                       const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    RungeKuttaGeometry(const RungeKuttaGeometry&) = default;

    /** @brief move constructor */
    RungeKuttaGeometry(RungeKuttaGeometry&&) = default;

    /** @brief copy assignment */
    RungeKuttaGeometry& operator=(const RungeKuttaGeometry&) = delete;

    /** @brief move assignment */
    RungeKuttaGeometry& operator=(RungeKuttaGeometry&&) = delete;

    ~RungeKuttaGeometry() override = default;

    /** @see Geometry::ray_from */
    std::unique_ptr<Ray<DIMENSION>>
    ray_from(const Vec<DIMENSION>& start,
             const Vec<DIMENSION>& direction) const override;

  public:
    /** @brief returns initial ray step size */
    Scalar ray_initial_step_size() const { return _ray_initial_step_size; }
    /** @brief returns absolute error for rays */
    Scalar ray_error_abs() const { return _ray_error_abs; }
    /** @brief returns relative error for rays */
    Scalar ray_error_rel() const { return _ray_error_rel; }
    /** @brief returnsmay length for rays  */
    Scalar ray_max_length() const { return _ray_max_length; }
    /** @brief returns ray segment scaling factor */
    Scalar ray_segment_length_factor() const {
        return _ray_segment_length_factor;
    }
    /** @brief returns ray differential */
    const RungeKuttaRayDifferential<DIMENSION>& phase_derivative_func() const {
        return _phase_derivative_func;
    }

    /** @brief returns Chirstoffel symbols of second kind  */
    virtual TenR3<DIMENSION>
    christoffel_2(const Vec<DIMENSION>& position) const = 0;

    /**
     * @brief returns true iff this ray segment should be exxtended to infinity
     * @note This is usefull if the geometry becomes flat outside a finite
     *       region.
     */
    virtual bool treat_as_infinite_segment(
        [[maybe_unused]] const Vec<DIMENSION>& position,
        [[maybe_unused]] const Vec<DIMENSION>& velocity) const {
        return false;
    }

  private:
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
    const RungeKuttaRayDifferential<DIMENSION> _phase_derivative_func;
};

template <Dimension DIMENSION>
void RungeKuttaRayDifferential<DIMENSION>::operator()(
    const Vec<2 * DIMENSION>& phase, Vec<2 * DIMENSION>& dphase_dtime,
    [[maybe_unused]] Scalar time) {
    using namespace tensor;

    const auto [pos, vel] = split(phase);
    const auto chris_2 = geometry.christoffel_2(pos);
    dphase_dtime = outer_sum(
        vel,
        gttl::contraction<1, 2>(gttl::contraction<1, 3>(chris_2, vel), -vel));
}

template <Dimension DIMENSION>
RungeKuttaRay<DIMENSION>::RungeKuttaRay(
    const RungeKuttaGeometry<DIMENSION>& geometry, const Vec<DIMENSION>& start,
    const Vec<DIMENSION>& direction)
    : _phase{tensor::outer_sum(start, direction)},
      _phase_iterator{make_phase_iterator(geometry, *this)},
      _geometry{geometry} {}

template <Dimension DIMENSION>
std::optional<RaySegment<DIMENSION>>
RungeKuttaRay<DIMENSION>::next_ray_segment() {
    using namespace tensor;

    const auto& [phase_start, time] = *_phase_iterator;
    // note: copies! (phase will be updated later)
    const Scalar time_start = time;

    // check for ray length
    if (time_start > _geometry.ray_max_length()) {
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

    if (_geometry.treat_as_infinite_segment(position, velocity)) {
        return RaySegment<DIMENSION>{position, velocity, infinity};
    }

    // update state
    ++_phase_iterator;

    // calculate segment
    // determine dt
    const auto& [_, time_end] = *_phase_iterator;
    // note: extend interval a tiny bit to avoid small scale geometrical banding
    const Scalar delta_t =
        (time_end - time_start) * _geometry.ray_segment_length_factor();
    // note: direction is approximately constant for small segments
    // note: use initial position and velocity
    const RaySegment<DIMENSION> segment = {position, velocity, delta_t};

    return segment;
};

template <Dimension DIMENSION>
typename RungeKuttaRay<DIMENSION>::Iterator
RungeKuttaRay<DIMENSION>::make_phase_iterator(
    const RungeKuttaGeometry<DIMENSION>& geometry, RungeKuttaRay& ray) {
    using namespace boost::numeric::odeint;

    const Scalar initial_dt = geometry.ray_initial_step_size();
    const Scalar error_abs = geometry.ray_error_abs();
    const Scalar error_rel = geometry.ray_error_rel();

    ControlledStepper stepper = make_controlled(
        error_abs, error_rel, runge_kutta_cash_karp54<State, Value>());

    return make_adaptive_time_iterator_begin<ControlledStepper, System, State>(
        std::move(stepper), geometry.phase_derivative_func(), ray._phase, 0.0,
        geometry.ray_max_length(), initial_dt);
}

template <Dimension DIMENSION>
RungeKuttaGeometry<DIMENSION>::RungeKuttaGeometry(
    const Scalar ray_initial_step_size, const Scalar ray_error_abs,
    const Scalar ray_error_rel, const Scalar ray_max_length,
    const Scalar ray_segment_length_factor)
    : _ray_initial_step_size(ray_initial_step_size),
      _ray_error_abs(ray_error_abs),
      _ray_error_rel(ray_error_rel),
      _ray_max_length(ray_max_length),
      _ray_segment_length_factor(ray_segment_length_factor),
      // note: store derivative function once per geometry
      _phase_derivative_func{*this} {}

template <Dimension DIMENSION>
std::unique_ptr<Ray<DIMENSION>>
RungeKuttaGeometry<DIMENSION>::ray_from(const Vec<DIMENSION>& start,
                                        const Vec<DIMENSION>& direction) const {
    return std::make_unique<RungeKuttaRay<DIMENSION>>(*this, start, direction);
}

} // namespace cpp_raytracing

#endif
