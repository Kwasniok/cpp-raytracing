#define BOOST_TEST_MODULE cpp_raytracing::values::color

#include <array>
#include <limits>

#include "../../common.hpp"

#include <cpp_raytracing/values/color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

const ray::ColorScalar epsilon = 1.e-12;

BOOST_AUTO_TEST_CASE(default_constructor, *but::tolerance(epsilon)) {
    ray::Color color{};
    std::array<ray::ColorScalar, 3> elems{0.0, 0.0, 0.0};
    TEST_EQUAL_RANGES(color, elems);
}

BOOST_AUTO_TEST_CASE(chanels_const, *but::tolerance(epsilon)) {
    const ray::Color color{1.1, 2.2, 3.3};
    BOOST_TEST(color.r() == 1.1);
    BOOST_TEST(color.g() == 2.2);
    BOOST_TEST(color.b() == 3.3);
}

BOOST_AUTO_TEST_CASE(chanels_mut, *but::tolerance(epsilon)) {
    ray::Color color;
    color.r() = 1.1;
    color.g() = 2.2;
    color.b() = 3.3;
    BOOST_TEST(color.r() == 1.1);
    BOOST_TEST(color.g() == 2.2);
    BOOST_TEST(color.b() == 3.3);
}

BOOST_AUTO_TEST_CASE(iterator_read_const, *but::tolerance(epsilon)) {
    const ray::Color color{1.1, 2.2, 3.3};
    std::array<ray::ColorScalar, 3> elems{1.1, 2.2, 3.3};
    TEST_EQUAL_RANGES(color, elems);
}

BOOST_AUTO_TEST_CASE(iterator_write_mut, *but::tolerance(epsilon)) {
    ray::Color color;
    std::array<ray::ColorScalar, 3> elems{1.1, 2.2, 3.3};
    auto it = std::begin(color);
    *(it++) = 1.1;
    *(it++) = 2.2;
    *(it++) = 3.3;
    TEST_EQUAL_RANGES(color, elems);
}

BOOST_AUTO_TEST_CASE(inplace_plus, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{5.5, 7.7, 9.9};
    ray::Color res = x1;
    res += x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(plus, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{5.5, 7.7, 9.9};
    ray::Color res = x1 + x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(inplace_minus, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{-3.3, -3.3, -3.3};
    ray::Color res = x1;
    res -= x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(minus, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{-3.3, -3.3, -3.3};
    ray::Color res = x1 - x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(inplace_scalar_mult, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::ColorScalar fac = 7.7;
    std::array<ray::ColorScalar, 3> elems{8.47, 16.94, 25.41};
    ray::Color res = x1;
    res *= fac;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(scalar_mult, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::ColorScalar fac = 7.7;
    std::array<ray::ColorScalar, 3> elems{8.47, 16.94, 25.41};
    ray::Color res;
    res = x1 * fac;

    TEST_EQUAL_RANGES(res, elems);
    res = fac * x1;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(inplace_scalar_div, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::ColorScalar fac = 7.7;
    std::array<ray::ColorScalar, 3> elems{1.1 / 7.7, 2.2 / 7.7, 3.3 / 7.7};
    ray::Color res = x1;
    res /= fac;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(scalar_div, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::ColorScalar fac = 7.7;
    std::array<ray::ColorScalar, 3> elems{1.1 / 7.7, 2.2 / 7.7, 3.3 / 7.7};
    ray::Color res;
    res = x1 / fac;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(inplace_elem_mult, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{4.84, 12.1, 21.78};
    ray::Color res = x1;
    res *= x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(elem_mult, *but::tolerance(epsilon)) {
    const ray::Color x1{1.1, 2.2, 3.3};
    const ray::Color x2{4.4, 5.5, 6.6};
    std::array<ray::ColorScalar, 3> elems{4.84, 12.1, 21.78};
    ray::Color res;
    res = x1 * x2;
    TEST_EQUAL_RANGES(res, elems);
}

BOOST_AUTO_TEST_CASE(int_from_color_scalar, *but::tolerance(epsilon)) {
    auto inf = std::numeric_limits<ray::ColorScalar>::infinity();
    auto quiet_NaN = std::numeric_limits<ray::ColorScalar>::quiet_NaN();
    auto signaling_NaN = std::numeric_limits<ray::ColorScalar>::signaling_NaN();

    ray::ColorIntegral res;

    res = ray::int_from_color_scalar(ray::ColorScalar{0.0});
    BOOST_CHECK(res == ray::ColorIntegral{0});

    res = ray::int_from_color_scalar(ray::ColorScalar{1.0});
    BOOST_CHECK(res == ray::ColorIntegral{255});

    res = ray::int_from_color_scalar(ray::ColorScalar{-3.0});
    BOOST_CHECK(res == ray::ColorIntegral{0});

    res = ray::int_from_color_scalar(ray::ColorScalar{+3.0});
    BOOST_CHECK(res == ray::ColorIntegral{255});

    res = ray::int_from_color_scalar(-inf);
    BOOST_CHECK(res == ray::ColorIntegral{0});

    res = ray::int_from_color_scalar(+inf);
    BOOST_CHECK(res == ray::ColorIntegral{255});

    res = ray::int_from_color_scalar(quiet_NaN);
    BOOST_CHECK(res == ray::ColorIntegral{0});

    res = ray::int_from_color_scalar(signaling_NaN);
    BOOST_CHECK(res == ray::ColorIntegral{0});
}

BOOST_AUTO_TEST_CASE(color_constants, *but::tolerance(epsilon)) {
    ray::Color color;
    std::array<ray::ColorScalar, 3> elems;

    color = ray::Colors::BLACK;
    elems = {0.0, 0.0, 0.0};
    TEST_EQUAL_RANGES(color, elems);

    color = ray::Colors::WHITE;
    elems = {1.0, 1.0, 1.0};
    TEST_EQUAL_RANGES(color, elems);

    color = ray::Colors::RED;
    elems = {1.0, 0.0, 0.0};
    TEST_EQUAL_RANGES(color, elems);

    color = ray::Colors::GREEN;
    elems = {0.0, 1.0, 0.0};
    TEST_EQUAL_RANGES(color, elems);

    color = ray::Colors::BLUE;
    elems = {0.0, 0.0, 1.0};
    TEST_EQUAL_RANGES(color, elems);
}
