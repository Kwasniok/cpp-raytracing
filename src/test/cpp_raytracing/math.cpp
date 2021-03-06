#include <array>
#include <limits>

#include <cpp_raytracing/math.hpp>
#include <cpp_raytracing/util.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.e-12;

void test_vec3_default_constructor() {
    Vec3 vec{};
    TEST_ASSERT_EQUAL(vec.x(), 0.0);
    TEST_ASSERT_EQUAL(vec.y(), 0.0);
    TEST_ASSERT_EQUAL(vec.z(), 0.0);
}

void test_vec3_random_constructor() {
    constexpr auto min = -1.1;
    constexpr auto max = 2.2;
    for (long i = 0; i < 1000; ++i) {
        Vec3 vec = Vec3::random(min, max);
        TEST_ASSERT_IN_RANGE(vec.x(), min, max);
        TEST_ASSERT_IN_RANGE(vec.y(), min, max);
        TEST_ASSERT_IN_RANGE(vec.z(), min, max);
    }
}

void test_vec3_axes() {
    Vec3 vec{1.1, 2.2, 3.3};
    TEST_ASSERT_EQUAL(vec.x(), 1.1);
    TEST_ASSERT_EQUAL(vec.y(), 2.2);
    TEST_ASSERT_EQUAL(vec.z(), 3.3);
}

void test_vec3_iterator() {
    // non-const (read)
    {
        Vec3 vec{1.1, 2.2, 3.3};
        std::array<Scalar, 3> elems{1.1, 2.2, 3.3};
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(vec, elems, epsilon);
    }
    // non-const (write)
    {
        Vec3 vec{1.1, 2.2, 3.3};
        std::array<Scalar, 3> elems = {7.7, 7.7, 7.7};
        for (auto& e : vec) {
            e = 7.7;
        }
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(vec, elems, epsilon);
    }
    // const
    {
        const Vec3 cvec{1.1, 2.2, 3.3};
        std::array<Scalar, 3> elems{1.1, 2.2, 3.3};
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(cvec, elems, epsilon);
    }
}

void test_vec3_comparison() {
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

void test_vec3_arithmetic() {
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

void test_vec3_near_zero() {
    TEST_ASSERT_TRUE(Vec3(1.0e-15, 1.0e-16, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-04, 1.0e-16, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-15, 1.0e-04, 1.0e-17).near_zero(epsilon));
    TEST_ASSERT_FALSE(Vec3(1.0e-15, 1.0e-16, 1.0e-04).near_zero(epsilon));
}

void test_vec3_random_in_unit_disk() {
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

void test_vec3_random_vector_in_unit_sphere() {
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

void test_vec3_random_unit_vector() {
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

void test_mat3x3_default_constructor() {
    const Mat3x3 mat{};
    TEST_ASSERT_EQUAL(mat.x(), (Vec3{1.0, 0.0, 0.0}));
    TEST_ASSERT_EQUAL(mat.y(), (Vec3{0.0, 1.0, 0.0}));
    TEST_ASSERT_EQUAL(mat.z(), (Vec3{0.0, 0.0, 1.0}));
}

void test_mat3x3_convention() {
    // row first
    constexpr Mat3x3 mat{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    constexpr Vec3 x{1.0, 0.0, 0.0};
    constexpr Vec3 y{0.0, 1.0, 0.0};
    constexpr Vec3 z{0.0, 0.0, 1.0};

    {
        // multiplication right
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(mat * x, (Vec3{1.1, 4.4, 7.7}),
                                          epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(mat * y, (Vec3{2.2, 5.5, 8.8}),
                                          epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(mat * z, (Vec3{3.3, 6.6, 9.9}),
                                          epsilon);
    }
    {
        // multiplication left
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(x * mat, (Vec3{1.1, 2.2, 3.3}),
                                          epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(y * mat, (Vec3{4.4, 5.5, 6.6}),
                                          epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(z * mat, (Vec3{7.7, 8.8, 9.9}),
                                          epsilon);
    }
}

void test_mat3x3_comparison() {
    constexpr Mat3x3 mat1{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    constexpr Mat3x3 mat2{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    constexpr Mat3x3 mat3{{1.1, 12.34, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    constexpr Mat3x3 mat4{{1.1, 2.2, 3.3}, {12.34, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    constexpr Mat3x3 mat5{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 12.34}};

    {
        TEST_ASSERT_TRUE(mat1 == mat1);
        TEST_ASSERT_TRUE(mat1 == mat2);
        TEST_ASSERT_FALSE(mat1 == mat3);
        TEST_ASSERT_FALSE(mat1 == mat4);
        TEST_ASSERT_FALSE(mat1 == mat5);
    }
    {
        TEST_ASSERT_FALSE(mat1 != mat1);
        TEST_ASSERT_FALSE(mat1 != mat2);
        TEST_ASSERT_TRUE(mat1 != mat3);
        TEST_ASSERT_TRUE(mat1 != mat4);
        TEST_ASSERT_TRUE(mat1 != mat5);
    }
}

void mat3x3_arithmetic() {
    constexpr Mat3x3 mat1{{1.9, 2.8, 3.7}, {4.6, 5.5, 6.4}, {3.7, 2.8, 1.9}};
    constexpr Mat3x3 mat2{{9.1, 8.2, 7.3}, {6.4, 5.5, 4.6}, {3.7, 2.8, 1.9}};
    constexpr Vec3 vec1{10.10, 11.11, 12.12};
    constexpr Scalar f = 1 / 11.0;

    {
        const auto res = mat1 + mat2;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 11.0, epsilon);
            }
        }
    }
    {
        auto res = mat1;
        res += mat2;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 11.0, epsilon);
            }
        }
    }
    {
        const auto res = mat1 - mat1;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 0.0, epsilon);
            }
        }
    }
    {
        auto res = mat1;
        res -= mat1;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 0.0, epsilon);
            }
        }
    }
    {
        const auto res = (mat1 + mat2) * f;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 1.0, epsilon);
            }
        }
    }
    {
        const auto res = f * (mat1 + mat2);
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 1.0, epsilon);
            }
        }
    }
    {
        auto res = (mat1 + mat2);
        res *= f;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], 1.0, epsilon);
            }
        }
    }
}

void test_mat3x3_rotation_mat() {
    constexpr Vec3 axis{1.1, 2.2, 3.3};
    constexpr Mat3x3 identity = Mat3x3{
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
    };

    const Mat3x3 mat = rotation_mat(axis);
    const Mat3x3 inv_mat = inverse_rotation_mat(axis);

    {
        // values
        constexpr Mat3x3 expected{
            {0.5811329478584188, -0.6399638097631108, -0.5027234021873948},
            {0.09283351724552164, -0.5615787520127085, 0.8221990290456893},
            {-0.8084964038195901, -0.524476527102342, -0.26694182424164375}};
        constexpr Scalar epsilon = 1e-14;
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(mat, expected, epsilon);
    }
    {
        // values of inverse
        constexpr Mat3x3 expected{
            {0.5811329478584188, 0.09283351724552164, -0.8084964038195901},
            {-0.6399638097631108, -0.5615787520127085, -0.524476527102342},
            {-0.5027234021873948, 0.8221990290456893, -0.26694182424164375}};
        constexpr Scalar epsilon = 1e-14;
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(inv_mat, expected, epsilon);
    }

    {
        // R * R^-1
        const Mat3x3 res = mat * inv_mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], identity[i][j], epsilon);
            }
        }
    }
    {
        // R^-1 * R
        const Mat3x3 res = inv_mat * mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], identity[i][j], epsilon);
            }
        }
    }
}

void test_mat3x3_scaling_mat() {
    constexpr Vec3 axis{1.1, 2.2, 3.3};
    constexpr Mat3x3 identity = Mat3x3{
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
    };

    const Mat3x3 mat = scaling_mat(axis);
    const Mat3x3 inv_mat = inverse_scaling_mat(axis);

    {
        const Mat3x3 res = mat * inv_mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], identity[i][j], epsilon);
            }
        }
    }
    {
        const Mat3x3 res = inv_mat * mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                TEST_ASSERT_ALMOST_EQUAL(res[i][j], identity[i][j], epsilon);
            }
        }
    }
}

void run_test_suite() {
    // vector
    run(test_vec3_default_constructor);
    run(test_vec3_random_constructor);
    run(test_vec3_axes);
    run(test_vec3_iterator);
    run(test_vec3_comparison);
    run(test_vec3_arithmetic);
    run(test_vec3_near_zero);
    run(test_vec3_random_in_unit_disk);
    run(test_vec3_random_vector_in_unit_sphere);
    run(test_vec3_random_unit_vector);

    // matrix
    run(test_mat3x3_default_constructor);
    run(test_mat3x3_convention);
    run(test_mat3x3_comparison);
    run(test_mat3x3_rotation_mat);
    run(test_mat3x3_scaling_mat);
}

}} // namespace cpp_raytracing::test
