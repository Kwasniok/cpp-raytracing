#include <cpp_raytracing/algorithm/runge_kutta.hpp>
#include <cpp_raytracing/values/tensor.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-12;

void test_runge_kutta_4_delta_basic() {
    // constant antiderivative/vanishing derivative

    const Vec6 x0 = {2, 3, 5, 7, 11, 13};
    const std::function<Vec6(const Vec6&)> f = [](const Vec6&) {
        return Vec6::zero();
    };

    Vec6 delta_x = runge_kutta_4_delta(f, x0, 1.0);

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x.first_half(), Vec3::zero(),
                                      epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x.second_half(), Vec3::zero(),
                                      epsilon);
}

void test_runge_kutta_4_delta_constant_velocity() {

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec6 p0 = {x0, v0};    // phase
    constexpr Scalar delta_t = 0.17;

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [](const Vec6& p) {
        return Vec6{p.second_half(), Vec3::zero()};
    };

    Vec6 delta_p = runge_kutta_4_delta(f, p0, delta_t);

    Vec3 delta_x = delta_p.first_half();
    Vec3 delta_v = delta_p.second_half();

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_x, v0 * delta_t, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_v, Vec3::zero(), epsilon);
}

void test_runge_kutta_4_delta_constant_force() {

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec3 a = {17, 19, 23}; // acceleration
    const Vec6 p0 = {x0, v0};    // phase
    constexpr Scalar delta_t = 0.29;

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [a = a](const Vec6& p) {
        return Vec6{p.second_half(), a};
    };

    Vec6 delta_p = runge_kutta_4_delta(f, p0, delta_t);

    Vec3 delta_x = delta_p.first_half();
    Vec3 delta_v = delta_p.second_half();

    // x1 = x0 + v0 * t + 0.5 * a * t ** 2
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(
        delta_x, (v0 + 0.5 * a * delta_t) * delta_t, epsilon);
    // v1 = v0 + a * t
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(delta_v, a * delta_t, epsilon);
}

void test_runge_kutta_4_delta_stability() {

    const Vec3 x0 = {2, 3, 5};   // position
    const Vec3 v0 = {7, 11, 13}; // velocity
    const Vec3 a = {17, 19, 23}; // acceleration

    // no acceleration
    const std::function<Vec6(const Vec6&)> f = [a = a](const Vec6& p) {
        return Vec6{p.second_half(), a};
    };

    // propagate over multiple steps
    constexpr int steps = 57;
    constexpr Scalar delta_t = 0.03;
    constexpr Scalar t = steps * delta_t;
    Vec6 p = {x0, v0}; // phase
    for (int i = 0; i < steps; ++i) {
        p = p + runge_kutta_4_delta(f, p, delta_t);
    }

    Vec3 x1 = p.first_half();
    Vec3 v1 = p.second_half();

    // x1 = x0 + v0 * t + 0.5 * a * t ** 2
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(x1, x0 + v0 * t + 0.5 * a * t * t,
                                      epsilon);
    // v1 = v0 + a * t
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(v1, v0 + a * t, epsilon);
}

void run_test_suite() {
    run(test_runge_kutta_4_delta_basic);
    run(test_runge_kutta_4_delta_constant_velocity);
    run(test_runge_kutta_4_delta_constant_force);
    run(test_runge_kutta_4_delta_stability);
}

}} // namespace cpp_raytracing::test
