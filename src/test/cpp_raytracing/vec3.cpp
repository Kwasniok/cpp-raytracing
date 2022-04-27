#include <limits>

#include <cpp_raytracing/vec3.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.e-12;

void test_default_constructor() {
    Vec3 vec{};
    TEST_ASSERT_EQUAL(vec.x(), 0.0);
    TEST_ASSERT_EQUAL(vec.y(), 0.0);
    TEST_ASSERT_EQUAL(vec.z(), 0.0);
}

void test_random_constructor() {
    constexpr auto min = -1.1;
    constexpr auto max = 2.2;
    for (long i = 0; i < 1000; ++i) {
        Vec3 vec = Vec3::random(min, max);
        TEST_ASSERT_IN_RANGE(vec.x(), min, max);
        TEST_ASSERT_IN_RANGE(vec.y(), min, max);
        TEST_ASSERT_IN_RANGE(vec.z(), min, max);
    }
}

void test_axes() {
    Vec3 vec{1.1, 2.2, 3.3};
    TEST_ASSERT_EQUAL(vec.x(), 1.1);
    TEST_ASSERT_EQUAL(vec.y(), 2.2);
    TEST_ASSERT_EQUAL(vec.z(), 3.3);
}

void test_comparison() {
    constexpr Vec3 vec1{1.1, 2.2, 3.3};
    constexpr Vec3 vec2{1.1, 2.2, 3.3};
    constexpr Vec3 vec3{7.7, 2.2, 3.3};
    constexpr Vec3 vec4{1.1, 7.7, 3.3};
    constexpr Vec3 vec5{1.1, 2.2, 7.7};

    {
        TEST_ASSERT_TRUE(vec1 == vec1);
        TEST_ASSERT_TRUE(vec1 == vec2);
        TEST_ASSERT_FALSE(vec1 == vec3);
        TEST_ASSERT_FALSE(vec1 == vec4);
        TEST_ASSERT_FALSE(vec1 == vec5);
    }
    {
        TEST_ASSERT_FALSE(vec1 != vec1);
        TEST_ASSERT_FALSE(vec1 != vec2);
        TEST_ASSERT_TRUE(vec1 != vec3);
        TEST_ASSERT_TRUE(vec1 != vec4);
        TEST_ASSERT_TRUE(vec1 != vec5);
    }
}

void test_arithmetic() {
    constexpr Vec3 x1{1.1, 2.2, 3.3};
    constexpr Vec3 x2{4.4, 5.5, 6.6};
    constexpr Scalar f = 7.7;

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
    {
        const auto res = dot(x1, x2);
        TEST_ASSERT_ALMOST_EQUAL(res, 1.1 * 4.4 + 2.2 * 5.5 + 3.3 * 6.6,
                                 epsilon);
    }
    {
        const auto res = cross(x1, x2);
        TEST_ASSERT_ALMOST_EQUAL(res[0], 2.2 * 6.6 - 3.3 * 5.5, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 3.3 * 4.4 - 1.1 * 6.6, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 1.1 * 5.5 - 2.2 * 4.4, epsilon);
    }
    {
        const auto res = unit_vector(x1);
        constexpr auto len = std::sqrt(1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3);
        TEST_ASSERT_ALMOST_EQUAL(res[0], 1.1 / len, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[1], 2.2 / len, epsilon);
        TEST_ASSERT_ALMOST_EQUAL(res[2], 3.3 / len, epsilon);
    }
    {
        const auto res = x1.length();
        constexpr auto len = std::sqrt(1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3);
        TEST_ASSERT_ALMOST_EQUAL(res, len, epsilon);
    }
    {
        const auto res = x1.length_squared();
        constexpr auto len_sq = 1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3;
        TEST_ASSERT_ALMOST_EQUAL(res, len_sq, epsilon);
    }
}

void test_near_zero() {
    TEST_ASSERT_TRUE(Vec3(1.0e-15, 1.0e-16, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-04, 1.0e-16, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-15, 1.0e-04, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-15, 1.0e-16, 1.0e-04).near_zero(epsilon));
}

void test_random_in_unit_disk() {
    constexpr auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_in_unit_disk();
        avg += vec;
        TEST_ASSERT_EQUAL(vec.z(), 0.0);
        TEST_ASSERT_IN_RANGE(vec.length(), 0.0, 1.0 + epsilon);
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    TEST_ASSERT_IN_RANGE(avg.length(), 0.0, 0.05);
}

void test_random_vector_in_unit_sphere() {
    constexpr auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_vector_in_unit_sphere();
        avg += vec;
        TEST_ASSERT_IN_RANGE(vec.length(), 0.0, 1.0 + epsilon);
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    TEST_ASSERT_IN_RANGE(avg.length(), 0.0, 0.05);
}

void test_random_unit_vector() {
    constexpr auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_unit_vector();
        avg += vec;
        TEST_ASSERT_ALMOST_EQUAL(vec.length(), 1.0, epsilon);
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    TEST_ASSERT_IN_RANGE(avg.length(), 0.0, 0.05);
}

void run_test_suite() {
    run(test_default_constructor);
    run(test_random_constructor);
    run(test_axes);
    run(test_comparison);
    run(test_arithmetic);
    run(test_near_zero);
    run(test_random_in_unit_disk);
    run(test_random_vector_in_unit_sphere);
    run(test_random_unit_vector);
}

}} // namespace cpp_raytracing::test
