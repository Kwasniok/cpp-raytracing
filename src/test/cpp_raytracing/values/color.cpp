#include <array>
#include <limits>

#include <cpp_raytracing/values/color.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr ColorScalar epsilon = 1.e-12;

void test_default_constructor() {
    Color color{};
    TEST_ASSERT_EQUAL(color.r(), 0.0);
    TEST_ASSERT_EQUAL(color.g(), 0.0);
    TEST_ASSERT_EQUAL(color.b(), 0.0);
}

void test_chanels() {
    Color color{1.1, 2.2, 3.3};
    TEST_ASSERT_EQUAL(color.r(), 1.1);
    TEST_ASSERT_EQUAL(color.g(), 2.2);
    TEST_ASSERT_EQUAL(color.b(), 3.3);
}

void test_iterator() {
    // non-const (read)
    {
        Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems{1.1, 2.2, 3.3};
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(color, elems, epsilon);
    }
    // non-const (write)
    {
        Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems = {7.7, 7.7, 7.7};
        for (auto& e : color) {
            e = 7.7;
        }
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(color, elems, epsilon);
    }
    // const
    {
        const Color color{1.1, 2.2, 3.3};
        std::array<ColorScalar, 3> elems{1.1, 2.2, 3.3};
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(color, elems, epsilon);
    }
}

void test_comparison() {
    constexpr Color color1{1.1, 2.2, 3.3};
    constexpr Color color2{1.1, 2.2, 3.3};
    constexpr Color color3{7.7, 2.2, 3.3};
    constexpr Color color4{1.1, 7.7, 3.3};
    constexpr Color color5{1.1, 2.2, 7.7};

    {
        TEST_ASSERT_TRUE(color1 == color1);
        TEST_ASSERT_TRUE(color1 == color2);
        TEST_ASSERT_FALSE(color1 == color3);
        TEST_ASSERT_FALSE(color1 == color4);
        TEST_ASSERT_FALSE(color1 == color5);
    }
    {
        TEST_ASSERT_FALSE(color1 != color1);
        TEST_ASSERT_FALSE(color1 != color2);
        TEST_ASSERT_TRUE(color1 != color3);
        TEST_ASSERT_TRUE(color1 != color4);
        TEST_ASSERT_TRUE(color1 != color5);
    }
}

void test_arithmetic() {
    constexpr Color x1{1.1, 2.2, 3.3};
    constexpr Color x2{4.4, 5.5, 6.6};
    constexpr ColorScalar f = 7.7;

    {
        const auto res = x1 + x2;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 5.5, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 9.9, epsilon);
    }
    {
        auto res = x1;
        res += x2;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 5.5, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 9.9, epsilon);
    }
    {
        const auto res = x2 - x1;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 3.3, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 3.3, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3, epsilon);
    }
    {
        auto res = x2;
        res -= x1;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 3.3, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 3.3, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3, epsilon);
    }
    {
        const auto res = x1 * f;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 * 7.7, epsilon);
    }
    {
        const auto res = f * x1;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 * 7.7, epsilon);
    }
    {
        auto res = x1;
        res *= f;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 * 7.7, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 * 7.7, epsilon);
    }
    {
        const auto res = x1 * x2;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 * 4.4, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 * 5.5, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 * 6.6, epsilon);
    }
    {
        auto res = x1;
        res *= x2;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 * 4.4, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 * 5.5, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 * 6.6, epsilon);
    }
    {
        const auto res = x1 / f;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.0 / 7.0, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.0 / 7.0, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.0 / 7.0, epsilon);
    }
    {
        auto res = x1;
        res /= f;
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.0 / 7.0, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.0 / 7.0, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.0 / 7.0, epsilon);
    }
}

void test_scalar_to_integral_conversion() {
    {
        const auto res = int_from_color_scalar(0.0);
        TEST_ASSERT_EQUAL(res, ColorIntegral(0));
    }
    {
        const auto res = int_from_color_scalar(1.0);
        TEST_ASSERT_EQUAL(res, ColorIntegral(255));
    }
    // clipping
    {
        const auto res = int_from_color_scalar(-3.0);
        TEST_ASSERT_EQUAL(res, ColorIntegral(0));
    }
    {
        const auto res = int_from_color_scalar(+3.0);
        TEST_ASSERT_EQUAL(res, ColorIntegral(255));
    }
    {
        const auto res = int_from_color_scalar(
            -std::numeric_limits<ColorScalar>::infinity());
        TEST_ASSERT_EQUAL(res, ColorIntegral(0));
    }
    {
        const auto res =
            int_from_color_scalar(std::numeric_limits<ColorScalar>::infinity());
        TEST_ASSERT_EQUAL(res, ColorIntegral(255));
    }
    {
        const auto res = int_from_color_scalar(
            std::numeric_limits<ColorScalar>::quiet_NaN());
        TEST_ASSERT_EQUAL(res, ColorIntegral(0));
    }
    {
        const auto res = int_from_color_scalar(
            std::numeric_limits<ColorScalar>::signaling_NaN());
        TEST_ASSERT_EQUAL(res, ColorIntegral(0));
    }
}

void test_color_constants() {
    {
        constexpr Color color = Colors::BLACK;
        TEST_ASSERT_EQUAL(color.r(), 0.0);
        TEST_ASSERT_EQUAL(color.g(), 0.0);
        TEST_ASSERT_EQUAL(color.b(), 0.0);
    }
    {
        constexpr Color color = Colors::WHITE;
        TEST_ASSERT_EQUAL(color.r(), 1.0);
        TEST_ASSERT_EQUAL(color.g(), 1.0);
        TEST_ASSERT_EQUAL(color.b(), 1.0);
    }
    {
        constexpr Color color = Colors::RED;
        TEST_ASSERT_EQUAL(color.r(), 1.0);
        TEST_ASSERT_EQUAL(color.g(), 0.0);
        TEST_ASSERT_EQUAL(color.b(), 0.0);
    }
    {
        constexpr Color color = Colors::GREEN;
        TEST_ASSERT_EQUAL(color.r(), 0.0);
        TEST_ASSERT_EQUAL(color.g(), 1.0);
        TEST_ASSERT_EQUAL(color.b(), 0.0);
    }
    {
        constexpr Color color = Colors::BLUE;
        TEST_ASSERT_EQUAL(color.r(), 0.0);
        TEST_ASSERT_EQUAL(color.g(), 0.0);
        TEST_ASSERT_EQUAL(color.b(), 1.0);
    }
}

void run_test_suite() {
    run(test_default_constructor);
    run(test_chanels);
    run(test_iterator);
    run(test_comparison);
    run(test_arithmetic);
    run(test_scalar_to_integral_conversion);
    run(test_color_constants);
}

}} // namespace cpp_raytracing::test
