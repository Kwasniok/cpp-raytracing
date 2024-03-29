#define BOOST_TEST_MODULE cpp_raytracing::values::tensor_boost_numeric_odeint

#include "../../common.hpp"

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

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

const ray::Scalar epsilon = 1e-3;
const ray::Scalar sigma = 10.0;
const ray::Scalar R = 28.0;
const ray::Scalar b = 8.0 / 3.0;

BOOST_AUTO_TEST_CASE(lorenz_attractor_vec3, *but::tolerance(epsilon)) {
    using namespace boost::numeric::odeint;
    using State = ray::Vec3;

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

    const ray::Vec3 expected = {11.7446, 17.6449, 15.9846};
    TEST_EQUAL_RANGES(*it, expected);
}
