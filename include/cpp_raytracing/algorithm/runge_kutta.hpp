/**
 * @file
 * @brief Runge Kutta algorithm
 */

#ifndef CPP_RAYTRACING_ALGORITHM_RUNGE_KUTTA_HPP
#define CPP_RAYTRACING_ALGORITHM_RUNGE_KUTTA_HPP

#include <functional>

namespace cpp_raytracing {

/**
 * @brief implementation of the Runge-Kutta algorithm to 4-th order
 * @return `delta_x` based on `delta_t` such that
 *         `delta_x / delta_t` approximates `f(x)`
 */
template <typename T, typename U>
T runge_kutta_4_delta(const std::function<T(const T&)>& f, const T& x,
                      const U delta_t) {
    k_1 = f(x);
    k_2 = f(x + k_1 * (delta_t / 2.0));
    k_3 = f(x + k_2 * (delta_t / 2.0));
    k_4 = f(x + k_3 * delta_t);
    return (k_1 + k_2 * 2.0 + k_3 * 2.0 + k_4) * (delta_t / 6.0);
}

} // namespace cpp_raytracing

#endif
