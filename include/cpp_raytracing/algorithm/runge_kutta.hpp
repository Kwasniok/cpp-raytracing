/**
 * @file
 * @brief Runge Kutta algorithm
 */

#ifndef CPP_RAYTRACING_ALGORITHM_RUNGE_KUTTA_HPP
#define CPP_RAYTRACING_ALGORITHM_RUNGE_KUTTA_HPP

#include <cmath>
#include <functional>

#include "../util.hpp"

namespace cpp_raytracing {

/**
 * @brief implementation of the Runge-Kutta algorithm to 4-th order
 * @tparam V vector type
 * @tparam S scalar type
 * @param f vector to vector function describing the first order derivative
 * @param x current vector
 * @param delta_t step_size of calculation
 * @return `delta_x` based on `delta_t` such that
 *         `delta_x / delta_t` approximates `f(x)`
 */
template <typename V, typename S>
V runge_kutta_4_delta(const std::function<V(const V&)>& f, const V& x,
                      const S delta_t) {
    const V k_1 = f(x);
    const V k_2 = f(x + k_1 * (delta_t / 2.0));
    const V k_3 = f(x + k_2 * (delta_t / 2.0));
    const V k_4 = f(x + k_3 * delta_t);
    return (k_1 + k_2 * 2.0 + k_3 * 2.0 + k_4) * (delta_t / 6.0);
}

/**
 *@brief Dormant-Prince solver for integrating time-independent differential
 *       equations
 */
template <typename V, typename S>
class RungeKutta_DoPri_45_Solver

{
  public:
    /** @brief type of derivative */
    using Func = std::function<V(const V&)>;

    /**
     * @tparam V vector type
     * @tparam S scalar type
     * @param f vector to vector function describing the derivative of `F`
     * @param max_error boundary for desired error estimate
     * @param initial_delta_t initial step size (step size is dynamic)
     * @param delta_t_min lower bound for delta_t
     * @param delta_t_max upper bound for delta_t
     */
    RungeKutta_DoPri_45_Solver(const Func& f, const S max_error,
                               const S initial_delta_t, const S delta_t_min,
                               const S delta_t_max)
        : f(f),
          max_error(max_error),
          delta_t(initial_delta_t),
          delta_t_min(delta_t_min),
          delta_t_max(delta_t_max) {}

    /**
     * @brief approximates `F(t + delta_t) - F(t)` based on `x = F(t)`
     * @note `delta_t` is automatically adapted towars an optimal value after
     *        each call.
     * @see delta_t
     */
    V delta(const V& x) {
        const V k_1 = delta_t * f(x);
        const V k_2 = delta_t * f(x + k_1 * (1.0 / 5.0));
        const V k_3 = delta_t * f(x + k_1 * (3.0 / 40.0) + k_2 * (9.0 / 40.0));
        const V k_4 = delta_t * f(x + k_1 * (44.0 / 45.0) -
                                  k_2 * (56.0 / 15.0) + k_3 * (32.0 / 9.0));
        const V k_5 =
            delta_t *
            f(x + k_1 * (19372.0 / 6561.0) - k_2 * (25360.0 / 2187.0) +
              k_3 * (64448.0 / 6561.0) - k_4 * (212.0 / 729.0));
        const V k_6 =
            delta_t * f(x + k_1 * (9017.0 / 3168.0) - k_2 * (355.0 / 33.0) +
                        k_3 * (46732.0 / 5247.0) + k_4 * (49.0 / 176.0) -
                        k_5 * (5103.0 / 18656.0));
        const V dx = (k_1 * (35.0 / 384.0) + k_3 * (500.0 / 1113.0) +
                      k_4 * (125.0 / 192.0) - k_5 * (2187.0 / 6784.0) +
                      k_6 * (11.0 / 84.0));
        const V k_7 = delta_t * f(x + dx);

        // estimated error
        const S error = (k_1 * (71.0 / 57600.0) - k_3 * (71.0 / 16695.0) +
                         k_4 * (71.0 / 1920.0) - k_5 * (17253.0 / 339200.0) +
                         k_6 * (22.0 / 525.0) - k_7 * (1.0 / 40.0))
                            .length();

        // adapt delta_t
        const S opt_delta_t =
            delta_t * std::pow(delta_t * max_error / (2.0 * error), 1.0 / 5.0);
        delta_t = clip(0.9 * opt_delta_t, delta_t_min, delta_t_max);

        return dx;
    }

  public:
    /** @brief derivative `f = F'` */
    const Func& f;
    /** @brief upper bound for estimated error */
    S max_error;
    /** @brief current step size */
    S delta_t;
    /** @brief upper bound for edelta_t */
    S delta_t_min;
    /** @brief lower bound for delta_t */
    S delta_t_max;
};

} // namespace cpp_raytracing

#endif
