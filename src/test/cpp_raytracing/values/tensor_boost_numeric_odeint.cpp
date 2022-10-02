#include <cpp_raytracing_test.hpp>

// note specific imports for spped and to avoid warnings for unrelated code
#include <boost/numeric/odeint/integrate/integrate_adaptive.hpp>
#include <boost/numeric/odeint/iterator/adaptive_iterator.hpp>
#include <boost/numeric/odeint/iterator/adaptive_time_iterator.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_controlled_runge_kutta.hpp>
#include <boost/numeric/odeint/stepper/generation/generation_runge_kutta_cash_karp54.hpp>
#include <boost/numeric/odeint/stepper/generation/make_controlled.hpp>
#include <boost/numeric/odeint/stepper/runge_kutta_cash_karp54.hpp>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/values/tensor_boost_numeric_odeint.hpp>

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1e-4;
const Scalar sigma = 10.0;
const Scalar R = 28.0;
const Scalar b = 8.0 / 3.0;

void test_vec2() {
    using namespace boost::numeric::odeint;
    using State = Vec2;

    State x = {10.0, 10.0};

    auto stepper =
        make_controlled(1.0e-6, 1.0e-6, runge_kutta_cash_karp54<State>());

    auto lorenz = [](const State& x, State& dxdt, [[maybe_unused]] double t) {
        dxdt[0] = sigma * (x[1] - x[0]);
        dxdt[1] = R * x[0] - x[1] - x[0];
    };

    auto it = make_adaptive_iterator_begin(stepper, lorenz, x, 0.0, 1.0, 0.01);
    ++it;
    ++it;
    ++it;

    const Vec2 expected = {12.2131, 21.9924};
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(*it, expected, epsilon);
}

void test_vec3() {
    using namespace boost::numeric::odeint;
    using State = Vec3;

    State x = {10.0, 10.0, 10.0};

    auto stepper =
        make_controlled(1.0e-6, 1.0e-6, runge_kutta_cash_karp54<State>());

    auto lorenz = [](const State& x, State& dxdt, [[maybe_unused]] double t) {
        dxdt[0] = sigma * (x[1] - x[0]);
        dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
        dxdt[2] = -b * x[2] + x[0] * x[1];
    };

    auto it = make_adaptive_iterator_begin(stepper, lorenz, x, 0.0, 1.0, 0.01);
    ++it;
    ++it;
    ++it;

    const Vec3 expected = {11.7446, 17.6449, 15.9846};
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(*it, expected, epsilon);
}

void test_vec6() {
    using namespace boost::numeric::odeint;
    using State = Vec6;

    State x = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0};

    auto stepper =
        make_controlled(1.0e-6, 1.0e-6, runge_kutta_cash_karp54<State>());

    auto lorenz = [](const State& x, State& dxdt, [[maybe_unused]] double t) {
        dxdt[0] = sigma * (x[1] - x[0]);
        dxdt[1] = R * x[0] - x[1] - x[0] * x[2];
        dxdt[2] = -b * x[2] + x[0] * x[1];
        //
        dxdt[3] = sigma * (x[4] - x[3]);
        dxdt[4] = R * x[3] - x[4] - x[3] * x[5];
        dxdt[5] = -b * x[5] + x[3] * x[4];
    };

    auto it = make_adaptive_iterator_begin(stepper, lorenz, x, 0.0, 1.0, 0.01);
    ++it;
    ++it;
    ++it;

    const Vec6 expected = {11.7446, 17.6449, 15.9846,
                           11.7446, 17.6449, 15.9846};
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(*it, expected, epsilon);
}

void run_test_suite() {
    // run(test);
    run(test_vec2);
    run(test_vec3);
    run(test_vec6);
}

}} // namespace cpp_raytracing::test
