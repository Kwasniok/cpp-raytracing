#include "../../common.hpp"

#include <array>
#include <limits>

#include <cpp_raytracing/values/color.hpp>

namespace cpp_raytracing { namespace test {

const ColorScalar epsilon = 1.e-12;

TEST_CASE("default_constructor") {
    Color color{};
    CHECK(color.r() == 0.0);
    CHECK(color.g() == 0.0);
    CHECK(color.b() == 0.0);
}

TEST_CASE("chanels") {
    Color color{1.1, 2.2, 3.3};
    CHECK(color.r() == 1.1);
    CHECK(color.g() == 2.2);
    CHECK(color.b() == 3.3);
}

TEST_CASE("iterator") {
    SUBCASE("mutalbe read") {
        Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems{1.1, 2.2, 3.3};
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, color, elems);
    }
    SUBCASE("mutable write") {
        Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems = {7.7, 7.7, 7.7};
        for (auto& e : color) {
            e = 7.7;
        }
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, color, elems);
    }
    SUBCASE("const") {
        const Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems{1.1, 2.2, 3.3};
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, color, elems);
    }
}

TEST_CASE("comparison") {
    const Color color1{1.1, 2.2, 3.3};
    const Color color2{1.1, 2.2, 3.3};
    const Color color3{7.7, 2.2, 3.3};
    const Color color4{1.1, 7.7, 3.3};
    const Color color5{1.1, 2.2, 7.7};

    {
        CHECK(color1 == color1);
        CHECK(color1 == color2);
        CHECK_FALSE(color1 == color3);
        CHECK_FALSE(color1 == color4);
        CHECK_FALSE(color1 == color5);
    }
    {
        CHECK_FALSE(color1 != color1);
        CHECK_FALSE(color1 != color2);
        CHECK(color1 != color3);
        CHECK(color1 != color4);
        CHECK(color1 != color5);
    }
}

TEST_CASE("arithmetic") {
    const Color x1{1.1, 2.2, 3.3};
    const Color x2{4.4, 5.5, 6.6};
    const ColorScalar f = 7.7;

    const Color sum{5.5, 7.7, 9.9};
    const Color diff{3.3, 3.3, 3.3};
    const Color prod{8.47, 16.94, 25.41};
    const Color sprod{4.84, 12.1, 21.78};
    const Color div{1.1 / 7.7, 2.2 / 7.7, 3.3 / 7.7};

    SUBCASE("add") {
        const auto res = x1 + x2;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, sum);
    }
    SUBCASE("implace add") {
        auto res = x1;
        res += x2;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, sum);
    }
    SUBCASE("subtract") {
        const auto res = x2 - x1;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, diff);
    }
    SUBCASE("inplace subtract") {
        auto res = x2;
        res -= x1;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, diff);
    }
    SUBCASE("multiply by scalar (right)") {
        const auto res = x1 * f;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, prod);
    }
    SUBCASE("multiply by scalar (left)") {
        const auto res = f * x1;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, prod);
    }
    SUBCASE("inplace multiply by scalar") {
        auto res = x1;
        res *= f;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, prod);
    }
    SUBCASE("scalar multiplication") {
        const auto res = x1 * x2;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, sprod);
    }
    SUBCASE("inplace scalar multiplication") {
        auto res = x1;
        res *= x2;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, sprod);
    }
    SUBCASE("scalar division") {
        const auto res = x1 / f;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, div);
    }
    SUBCASE("inplace scalar division") {
        auto res = x1;
        res /= f;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, res, div);
    }
}

TEST_CASE("scalar to integral conversion") {
    SUBCASE("zero") {
        const auto res = int_from_color_scalar(0.0);
        CHECK(res == ColorIntegral(0));
    }
    SUBCASE("one") {
        const auto res = int_from_color_scalar(1.0);
        CHECK(res == ColorIntegral(255));
    }
    SUBCASE("clipping low") {
        const auto res = int_from_color_scalar(-3.0);
        CHECK(res == ColorIntegral(0));
    }
    SUBCASE("clipping high") {
        const auto res = int_from_color_scalar(+3.0);
        CHECK(res == ColorIntegral(255));
    }
    SUBCASE("clipping -infinity") {
        const auto res = int_from_color_scalar(
            -std::numeric_limits<ColorScalar>::infinity());
        CHECK(res == ColorIntegral(0));
    }
    SUBCASE("clipping infinity") {
        const auto res =
            int_from_color_scalar(std::numeric_limits<ColorScalar>::infinity());
        CHECK(res == ColorIntegral(255));
    }
    SUBCASE("quiet nan") {
        const auto res = int_from_color_scalar(
            std::numeric_limits<ColorScalar>::quiet_NaN());
        CHECK(res == ColorIntegral(0));
    }
    SUBCASE("signaling nan") {
        const auto res = int_from_color_scalar(
            std::numeric_limits<ColorScalar>::signaling_NaN());
        CHECK(res == ColorIntegral(0));
    }
}

TEST_CASE("color constants") {
    SUBCASE("black") {
        const Color color = Colors::BLACK;
        CHECK(color.r() == 0.0);
        CHECK(color.g() == 0.0);
        CHECK(color.b() == 0.0);
    }
    SUBCASE("white") {
        const Color color = Colors::WHITE;
        CHECK(color.r() == 1.0);
        CHECK(color.g() == 1.0);
        CHECK(color.b() == 1.0);
    }
    SUBCASE("red") {
        const Color color = Colors::RED;
        CHECK(color.r() == 1.0);
        CHECK(color.g() == 0.0);
        CHECK(color.b() == 0.0);
    }
    SUBCASE("green") {
        const Color color = Colors::GREEN;
        CHECK(color.r() == 0.0);
        CHECK(color.g() == 1.0);
        CHECK(color.b() == 0.0);
    }
    SUBCASE("blue") {
        const Color color = Colors::BLUE;
        CHECK(color.r() == 0.0);
        CHECK(color.g() == 0.0);
        CHECK(color.b() == 1.0);
    }
}

}} // namespace cpp_raytracing::test
