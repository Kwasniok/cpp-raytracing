#define BOOST_TEST_MODULE cpp_raytracing::values::tensor

#include "../../common.hpp"

#include <cpp_raytracing/util.hpp>
#include <cpp_raytracing/values/tensor.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

constexpr ray::Scalar epsilon = 1.e-12;

BOOST_AUTO_TEST_CASE(is_zero_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::is_zero;
    BOOST_CHECK(is_zero(ray::Vec3{+0.0, +0.0, +0.0}));
    BOOST_CHECK(is_zero(ray::Vec3{-0.0, +0.0, +0.0}));
    BOOST_CHECK(is_zero(ray::Vec3{+0.0, -0.0, +0.0}));
    BOOST_CHECK(is_zero(ray::Vec3{+0.0, +0.0, -0.0}));
    BOOST_CHECK(!is_zero(ray::Vec3{1.0e-15, 1.0e-16, 1.0e-17}));
}

BOOST_AUTO_TEST_CASE(near_zero_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::near_zero;
    BOOST_CHECK(near_zero(ray::Vec3{1.0e-15, 1.0e-16, 1.0e-17}, epsilon));
    BOOST_CHECK(!near_zero(ray::Vec3{1.0e-04, 1.0e-16, 1.0e-17}, epsilon));
    BOOST_CHECK(!near_zero(ray::Vec3{1.0e-15, 1.0e-04, 1.0e-17}, epsilon));
    BOOST_CHECK(!near_zero(ray::Vec3{1.0e-15, 1.0e-16, 1.0e-04}, epsilon));
}

BOOST_AUTO_TEST_CASE(split_vec6, *but::tolerance(epsilon)) {
    using ray::tensor::split;
    const ray::Vec6 v6{1, 2, 3, 4, 5, 6};
    auto [v1, v2] = split(v6);

    const ray::Vec3 v1_expect{1, 2, 3};
    const ray::Vec3 v2_expect{4, 5, 6};
    TEST_EQUAL_RANGES(v1, v1_expect);
    TEST_EQUAL_RANGES(v2, v2_expect);
}

BOOST_AUTO_TEST_CASE(outer_sum_vec3_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::outer_sum;
    const ray::Vec3 v1{1, 2, 3};
    const ray::Vec3 v2{4, 5, 6};
    ray::Vec6 v6 = outer_sum(v1, v2);

    const ray::Vec6 v6_expect{1, 2, 3, 4, 5, 6};
    TEST_EQUAL_RANGES(v6, v6_expect);
}

BOOST_AUTO_TEST_CASE(dot_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::dot;
    const ray::Vec3 v0{0.0, 0.0, 0.0};
    const ray::Vec3 v1{1.0, -2.0, 3.0};
    const ray::Vec3 v2{4.0, 5.0, 6.0};

    BOOST_TEST(dot(v0, v1) == 0.0);
    BOOST_TEST(dot(v1, v0) == 0.0);
    BOOST_TEST(dot(v1, v2) == 12.0);
    BOOST_TEST(dot(v2, v1) == 12.0);
}

BOOST_AUTO_TEST_CASE(cross_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::cross;

    const ray::Vec3 v0{0.0, 0.0, 0.0};
    const ray::Vec3 v1{1.0, -2.0, 3.0};
    const ray::Vec3 v2{4.0, 5.0, 6.0};
    const ray::Vec3 v3{-27.0, 6.0, 13.0};

    TEST_EQUAL_RANGES(cross(v0, v1), v0);
    TEST_EQUAL_RANGES(cross(v1, v0), v0);
    TEST_EQUAL_RANGES(cross(v1, v1), v0);
    TEST_EQUAL_RANGES(cross(v1, v2), v3);
    TEST_EQUAL_RANGES(cross(v2, v1), -v3);
}

BOOST_AUTO_TEST_CASE(length_squared_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::length_squared;
    const ray::Vec3 v0{0.0, 0.0, 0.0};
    const ray::Vec3 v1{1.0, -2.0, 3.0};

    BOOST_TEST(length_squared(v0) == 0.0);
    BOOST_TEST(length_squared(v1) == 14.0);
}

BOOST_AUTO_TEST_CASE(length_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::length;
    const ray::Vec3 v0{0.0, 0.0, 0.0};
    const ray::Vec3 v1{1.0, -2.0, 3.0};

    BOOST_TEST(length(v0) == 0.0);
    BOOST_TEST(length(v1) == std::sqrt(14.0));
}

BOOST_AUTO_TEST_CASE(unit_vector_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector;

    const ray::Vec3 v1{1.0, 0.0, 0.0};
    const ray::Vec3 v2{0.0, 1.0, 0.0};
    const ray::Vec3 v3{0.0, 0.0, 1.0};
    const ray::Vec3 v4{+1.0, -2.0, +3.0};

    ray::Vec3 expect;

    TEST_EQUAL_RANGES(unit_vector(v1), v1);
    TEST_EQUAL_RANGES(unit_vector(v2), v2);
    TEST_EQUAL_RANGES(unit_vector(v3), v3);

    expect = ray::Vec3{+1.0, -2.0, +3.0} * (1 / std::sqrt(14.0));
    TEST_EQUAL_RANGES(unit_vector(v4), expect);
}

BOOST_AUTO_TEST_CASE(random_vec_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::random_vec;
    const auto min = -1.1;
    const auto max = 2.2;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        const ray::Vec3 vec = random_vec<3_D>(min, max);
        CHECK_IN_BOUNDS(vec[0], min, max);
        CHECK_IN_BOUNDS(vec[0], min, max);
        CHECK_IN_BOUNDS(vec[0], min, max);
    }
}

BOOST_AUTO_TEST_CASE(random_vec_inside_unit_sphere_vec2,
                     *but::tolerance(epsilon)) {
    using ray::tensor::random_vec_inside_unit_sphere, ray::tensor::length;
    const auto reps = 1000;
    ray::Vec2 avg{};
    for (auto i = 0; i < reps; ++i) {
        const ray::Vec2 vec = random_vec_inside_unit_sphere<2_D>();
        avg += vec;
        CHECK_IN_BOUNDS(length(vec), 0.0, 1.0 + epsilon);
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_BOUNDS(length(avg), 0.0, 0.05);
}

BOOST_AUTO_TEST_CASE(random_vec_inside_unit_sphere_vec3,
                     *but::tolerance(epsilon)) {
    using ray::tensor::random_vec_inside_unit_sphere, ray::tensor::length;
    const auto reps = 1000;
    ray::Vec3 avg{};
    for (auto i = 0; i < reps; ++i) {
        const ray::Vec3 vec = random_vec_inside_unit_sphere<3_D>();
        avg += vec;
        CHECK_IN_BOUNDS(length(vec), 0.0, 1.0 + epsilon);
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_BOUNDS(length(avg), 0.0, 0.05);
}

BOOST_AUTO_TEST_CASE(random_unit_vec_vec3, *but::tolerance(epsilon)) {
    using ray::tensor::random_unit_vec, ray::tensor::length;
    const auto reps = 1000;
    ray::Vec3 avg{};
    for (auto i = 0; i < reps; ++i) {
        const ray::Vec3 vec = random_unit_vec<3_D>();
        avg += vec;
        BOOST_TEST(length(vec) == 1.0);
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_BOUNDS(length(avg), 0.0, 0.05);
}

BOOST_AUTO_TEST_CASE(get_coords_in_plane_2d, *but::tolerance(epsilon)) {
    using ray::tensor::get_coords_in_plane;

    const std::vector<
        std::tuple<ray::Vec<2_D>, ray::Vec<2_D>, ray::Vec<2_D>, ray::Vec<2_D>>>
        args_and_expects{
            {
                {1.0, 0.0},
                {0.0, 1.0},
                {1.0, 2.0},
                {1.0, 2.0},
            },
            {
                {2.0, 3.0},
                {-5.0, 7.0},
                {11.0, 13.0},
                {142.0 / 29.0, -7.0 / 29.0},
            },
        };

    for (const auto& [vec1, vec2, point, expect] : args_and_expects) {
        ray::Vec<2_D> res = get_coords_in_plane<2>(vec1, vec2, point);
        TEST_EQUAL_RANGES(res, expect);
    }
}

BOOST_AUTO_TEST_CASE(square_mat_vec_mult, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat3x3 mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };
    const ray::Vec3 x{1.0, 0.0, 0.0};
    const ray::Vec3 y{0.0, 1.0, 0.0};
    const ray::Vec3 z{0.0, 0.0, 1.0};

    ray::Vec3 expect;
    {
        // multiplication right
        expect = {1.1, 4.4, 7.7};
        TEST_EQUAL_RANGES(mat * x, expect);
        expect = {2.2, 5.5, 8.8};
        TEST_EQUAL_RANGES(mat * y, expect);
        expect = {3.3, 6.6, 9.9};
        TEST_EQUAL_RANGES(mat * z, expect);
    }

    {
        // multiplication left
        expect = {1.1, 2.2, 3.3};
        TEST_EQUAL_RANGES(x * mat, expect);
        expect = {4.4, 5.5, 6.6};
        TEST_EQUAL_RANGES(y * mat, expect);
        expect = {7.7, 8.8, 9.9};
        TEST_EQUAL_RANGES(z * mat, expect);
    }
}

BOOST_AUTO_TEST_CASE(non_square_mat_vec_mult, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<2_D, 3_D> mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        // clang-format on
    };
    const ray::Vec<3_D> x{1.0, 0.0, 0.0};
    const ray::Vec<3_D> y{0.0, 1.0, 0.0};
    const ray::Vec<3_D> z{0.0, 0.0, 1.0};

    ray::Vec<2_D> expect;
    expect = {1.1, 4.4};
    TEST_EQUAL_RANGES(mat * x, expect);
    expect = {2.2, 5.5};
    TEST_EQUAL_RANGES(mat * y, expect);
    expect = {3.3, 6.6};
    TEST_EQUAL_RANGES(mat * z, expect);
}

BOOST_AUTO_TEST_CASE(vec_non_square_mat_mult, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<3_D, 2_D> mat{
        // clang-format off
        1.1, 2.2,
        4.4, 5.5,
        7.7, 8.8,
        // clang-format on
    };
    const ray::Vec<3_D> x{1.0, 0.0, 0.0};
    const ray::Vec<3_D> y{0.0, 1.0, 0.0};
    const ray::Vec<3_D> z{0.0, 0.0, 1.0};

    ray::Vec<2_D> expect;
    expect = {1.1, 2.2};
    TEST_EQUAL_RANGES(x * mat, expect);
    expect = {4.4, 5.5};
    TEST_EQUAL_RANGES(y * mat, expect);
    expect = {7.7, 8.8};
    TEST_EQUAL_RANGES(z * mat, expect);
}

BOOST_AUTO_TEST_CASE(embeded_vector_middle, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    const ray::Vec<3_D> vec{1.1, 2.2, 3.3};
    ray::Vec<5_D> expect{0.0, 1.1, 2.2, 3.3, 0.0};

    ray::Vec<5_D> res = embeded_vector<5, 1, 3>(vec);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(embeded_vector_left, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    const ray::Vec<3_D> vec{1.1, 2.2, 3.3};
    ray::Vec<5_D> expect{1.1, 2.2, 3.3, 0.0, 0.0};

    ray::Vec<5_D> res = embeded_vector<5, 0, 3>(vec);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(embeded_vector_right, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    const ray::Vec<3_D> vec{1.1, 2.2, 3.3};
    ray::Vec<5_D> expect{0.0, 0.0, 1.1, 2.2, 3.3};

    ray::Vec<5_D> res = embeded_vector<5, 2, 3>(vec);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(embedded_vector_exact, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    const ray::Vec<3_D> vec{1.1, 2.2, 3.3};

    ray::Vec<3_D> res = embeded_vector<3, 0, 3>(vec);
    TEST_EQUAL_RANGES(res, vec);
}

BOOST_AUTO_TEST_CASE(embeded_matrix_middle, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<3_D, 3_D> mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };

    ray::Mat<5_D, 5_D> expect{
        // clang-format off
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.1, 2.2, 3.3, 0.0,
        0.0, 4.4, 5.5, 6.6, 0.0,
        0.0, 7.7, 8.8, 9.9, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        // clang-format on
    };

    ray::Mat<5_D, 5_D> res = embeded_matrix<5, 5, 1, 1, 3, 3>(mat);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(embeded_matrix_left, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<3_D, 3_D> mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };

    ray::Mat<5_D, 5_D> expect{
        // clang-format off
        0.0, 0.0, 0.0, 0.0, 0.0,
        1.1, 2.2, 3.3, 0.0, 0.0,
        4.4, 5.5, 6.6, 0.0, 0.0,
        7.7, 8.8, 9.9, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        // clang-format on
    };

    ray::Mat<5_D, 5_D> res = embeded_matrix<5, 5, 1, 0, 3, 3>(mat);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(embeded_matrix_exact, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<3_D, 3_D> mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };

    ray::Mat<3_D, 3_D> res = embeded_matrix<3, 3, 0, 0, 3, 3>(mat);
    TEST_EQUAL_RANGES(res, mat);
}

BOOST_AUTO_TEST_CASE(embeded_matrix_left_upper, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    // row first
    const ray::Mat<3_D, 3_D> mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };

    ray::Mat<5_D, 5_D> expect{
        // clang-format off
        1.1, 2.2, 3.3, 0.0, 0.0,
        4.4, 5.5, 6.6, 0.0, 0.0,
        7.7, 8.8, 9.9, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, 0.0,
        // clang-format on
    };

    ray::Mat<5_D, 5_D> res = embeded_matrix<5, 5, 0, 0, 3, 3>(mat);
    TEST_EQUAL_RANGES(res, expect);
}

BOOST_AUTO_TEST_CASE(mat3x3_rotation_mat, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    using ray::tensor::rotation_mat, ray::tensor::inverse_rotation_mat;

    const ray::Vec3 axis{1.1, 2.2, 3.3};
    const ray::Mat3x3 identity = ray::Mat3x3{
        // clang-format off
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // clang-format on
    };
    const ray::Mat3x3 rotation_expect = ray::Mat3x3{
        // clang-format off
        0.5811329478584188, -0.6399638097631108, -0.5027234021873948,
        0.09283351724552164, -0.5615787520127085, 0.8221990290456893,
        -0.8084964038195901, -0.524476527102342, -0.26694182424164375,
        // clang-format on
    };

    const ray::Mat3x3 mat = rotation_mat(axis);
    const ray::Mat3x3 inv_mat = inverse_rotation_mat(axis);

    TEST_EQUAL_RANGES(mat, rotation_expect);
    TEST_EQUAL_RANGES(mat * inv_mat, identity);
    TEST_EQUAL_RANGES(inv_mat * mat, identity);
}

BOOST_AUTO_TEST_CASE(mat3x3_scaling_mat, *but::tolerance(epsilon)) {
    using namespace ray::tensor;
    using ray::tensor::scaling_mat, ray::tensor::inverse_scaling_mat;

    const ray::Vec3 axis{1.1, 2.2, 3.3};
    const ray::Mat3x3 identity = ray::Mat3x3{
        // clang-format off
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // clang-format on
    };
    const ray::Mat3x3 scaling_expect = ray::Mat3x3{
        // clang-format off
        1.1, 0.0, 0.0,
        0.0, 2.2, 0.0,
        0.0, 0.0, 3.3,
        // clang-format on
    };

    const ray::Mat3x3 mat = scaling_mat<3_D>(axis);
    const ray::Mat3x3 inv_mat = inverse_scaling_mat<3_D>(axis);

    TEST_EQUAL_RANGES(mat, scaling_expect);
    TEST_EQUAL_RANGES(mat * inv_mat, identity);
    TEST_EQUAL_RANGES(inv_mat * mat, identity);
}
