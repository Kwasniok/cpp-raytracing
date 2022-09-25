#include <cpp_raytracing/algorithm/runge_kutta.hpp>
#include <cpp_raytracing/values/tensor.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-12;

void test_runge_kutta_dopri_45_delta_basic() {

    constexpr Scalar max_error = 1e-8;
    constexpr Scalar delta_t_min = 1e-3;
    constexpr Scalar delta_t_max = 1e+3;

    const Vec6 x0 = {2, 3, 5, 7, 11, 13};
    constexpr Scalar delta_t = 1.0;

    // no change at all
    const std::function<Vec6(const Vec6&)> f = [](const Vec6&) {
        return Vec6::zero();
    };

    RungeKutta_DoPri_45_Solver solver = {f, max_error, delta_t, delta_t_min,
                                         delta_t_max};
    const Vec6 delta_x = solver.delta(x0);

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x.first_half(), Vec3::zero(),
                                      epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x.second_half(), Vec3::zero(),
                                      epsilon);
}

void test_runge_kutta_dopri_45_delta_constant_velocity() {

    constexpr Scalar max_error = 1e-8;
    constexpr Scalar delta_t_min = 1e-3;
    constexpr Scalar delta_t_max = 1e+3;

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec6 p0 = {x0, v0};    // phase
    constexpr Scalar delta_t = 0.17;

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [](const Vec6& p) {
        return Vec6{p.second_half(), Vec3::zero()};
    };

    RungeKutta_DoPri_45_Solver solver = {f, max_error, delta_t, delta_t_min,
                                         delta_t_max};
    Vec6 delta_p = solver.delta(p0);

    Vec3 delta_x = delta_p.first_half();
    Vec3 delta_v = delta_p.second_half();

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x, v0 * delta_t, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_v, Vec3::zero(), epsilon);
}

void test_runge_kutta_dopri_45_delta_constant_force() {

    constexpr Scalar max_error = 1e-8;
    constexpr Scalar delta_t_min = 1e-3;
    constexpr Scalar delta_t_max = 1e+3;

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec3 a = {17, 19, 23}; // acceleration
    const Vec6 p0 = {x0, v0};    // phase
    constexpr Scalar delta_t = 0.29;

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [a = a](const Vec6& p) {
        return Vec6{p.second_half(), a};
    };

    RungeKutta_DoPri_45_Solver solver = {f, max_error, delta_t, delta_t_min,
                                         delta_t_max};
    Vec6 delta_p = solver.delta(p0);

    Vec3 delta_x = delta_p.first_half();
    Vec3 delta_v = delta_p.second_half();

    // x1 = x0 + v0 * t + 0.5 * a * t ** 2
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(
        delta_x, (v0 + 0.5 * a * delta_t) * delta_t, epsilon);
    // v1 = v0 + a * t
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_v, a * delta_t, epsilon);
}

void test_runge_kutta_dopri_45_delta_stability() {

    constexpr Scalar max_error = 1e-8;
    constexpr Scalar delta_t_min = 1e-3;
    constexpr Scalar delta_t_max = 1e+3;

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec3 a = {17, 19, 23}; // acceleration

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [a = a](const Vec6& p) {
        return Vec6{p.second_half(), a};
    };

    // propagate over multiple steps
    constexpr int steps = 57;
    constexpr Scalar initial_delta_t = 0.03;
    Vec6 p = {x0, v0}; // phase

    RungeKutta_DoPri_45_Solver solver = {f, max_error, initial_delta_t,
                                         delta_t_min, delta_t_max};
    for (int i = 0; i < steps; ++i) {
        p += solver.delta(p);
    }

    Vec3 x1 = p.first_half();
    Vec3 v1 = p.second_half();

    // v1 = v0 + a * t (where t is unknown)
    const Scalar t = (v1 - v0)[2] / a[2];
    // x1 = x0 + v0 * t + 0.5 * a * t ** 2 (where t is unknown)
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(x1, x0 + v0 * t + 0.5 * a * t * t,
                                      max_error * 1e4);
    // v1 = v0 + a * t
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(v1, v0 + a * t, max_error * 1e4);
}

void run_test_suite() {
    run(test_runge_kutta_dopri_45_delta_basic);
    run(test_runge_kutta_dopri_45_delta_constant_velocity);
    run(test_runge_kutta_dopri_45_delta_constant_force);
    run(test_runge_kutta_dopri_45_delta_stability);
}

}} // namespace cpp_raytracing::test
