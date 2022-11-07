#include "../../common.hpp"

#include <array>
#include <limits>

#include <cpp_raytracing/util.hpp>
#include <cpp_raytracing/values/tensor.hpp>

namespace cpp_raytracing { namespace tensor { namespace test {

const Scalar epsilon = 1.e-12;

TEST_CASE("is_zero_vec3") {
    CHECK(is_zero(Vec3{+0.0, +0.0, +0.0}));
    CHECK(is_zero(Vec3{-0.0, +0.0, +0.0}));
    CHECK(is_zero(Vec3{+0.0, -0.0, +0.0}));
    CHECK(is_zero(Vec3{+0.0, +0.0, -0.0}));
    CHECK_FALSE(is_zero(Vec3{1.0e-15, 1.0e-16, 1.0e-17}));
}

TEST_CASE("near_zero_vec3") {
    CHECK(near_zero(Vec3{1.0e-15, 1.0e-16, 1.0e-17}, epsilon));
    CHECK_FALSE(near_zero(Vec3{1.0e-04, 1.0e-16, 1.0e-17}, epsilon));
    CHECK_FALSE(near_zero(Vec3{1.0e-15, 1.0e-04, 1.0e-17}, epsilon));
    CHECK_FALSE(near_zero(Vec3{1.0e-15, 1.0e-16, 1.0e-04}, epsilon));
}

TEST_CASE("split_vec6") {
    const Vec6 v6{1, 2, 3, 4, 5, 6};
    auto [v1, v2] = split(v6);

    CHECK(v1[0] == doctest::Approx(1).epsilon(epsilon));
    CHECK(v1[1] == doctest::Approx(2).epsilon(epsilon));
    CHECK(v1[2] == doctest::Approx(3).epsilon(epsilon));
    CHECK(v2[0] == doctest::Approx(4).epsilon(epsilon));
    CHECK(v2[1] == doctest::Approx(5).epsilon(epsilon));
    CHECK(v2[2] == doctest::Approx(6).epsilon(epsilon));
}

TEST_CASE("outer_sum_vec3_vec3") {
    const Vec3 v1{1, 2, 3};
    const Vec3 v2{4, 5, 6};
    Vec6 v6 = outer_sum(v1, v2);

    CHECK(v6[0] == doctest::Approx(1).epsilon(epsilon));
    CHECK(v6[1] == doctest::Approx(2).epsilon(epsilon));
    CHECK(v6[2] == doctest::Approx(3).epsilon(epsilon));
    CHECK(v6[3] == doctest::Approx(4).epsilon(epsilon));
    CHECK(v6[4] == doctest::Approx(5).epsilon(epsilon));
    CHECK(v6[5] == doctest::Approx(6).epsilon(epsilon));
}

TEST_CASE("dot_vec3") {
    const Vec3 v0{0.0, 0.0, 0.0};
    const Vec3 v1{1.0, -2.0, 3.0};
    const Vec3 v2{4.0, 5.0, 6.0};

    CHECK(dot(v0, v1) == 0.0);
    CHECK(dot(v1, v0) == 0.0);
    CHECK(dot(v1, v2) == doctest::Approx(12.0).epsilon(epsilon));
    CHECK(dot(v2, v1) == doctest::Approx(12.0).epsilon(epsilon));
}

TEST_CASE("cross_vec3") {
    const Vec3 v0{0.0, 0.0, 0.0};
    const Vec3 v1{1.0, -2.0, 3.0};
    const Vec3 v2{4.0, 5.0, 6.0};
    const Vec3 v3{-27.0, 6.0, 13.0};

    for (auto i = 0; i < 3; ++i) {
        CHECK(cross(v0, v1)[i] == 0.0);
    }
    for (auto i = 0; i < 3; ++i) {
        CHECK(cross(v1, v0)[i] == 0.0);
    }
    for (auto i = 0; i < 3; ++i) {
        CHECK(cross(v1, v2)[i] == v3[i]);
    }
    for (auto i = 0; i < 3; ++i) {
        CHECK(cross(v2, v1)[i] == -v3[i]);
    }
}

TEST_CASE("length_squared_vec3") {
    const Vec3 v0{0.0, 0.0, 0.0};
    const Vec3 v1{1.0, -2.0, 3.0};

    CHECK(length_squared(v0) == 0.0);
    CHECK(length_squared(v1) == doctest::Approx(14.0).epsilon(epsilon));
}

TEST_CASE("length_vec3") {
    const Vec3 v0{0.0, 0.0, 0.0};
    const Vec3 v1{1.0, -2.0, 3.0};

    CHECK(length(v0) == 0.0);
    CHECK(length(v1) == doctest::Approx(std::sqrt(14.0)).epsilon(epsilon));
}

TEST_CASE("unit_vector_vec3") {
    const Vec3 v1{1.0, 0.0, 0.0};
    const Vec3 v2{0.0, 1.0, 0.0};
    const Vec3 v3{0.0, 0.0, 1.0};
    const Vec3 v4{+1.0, -2.0, +3.0};

    CHECK(unit_vector(v1)[0] == 1.0);
    CHECK(unit_vector(v1)[1] == 0.0);
    CHECK(unit_vector(v1)[2] == 0.0);
    CHECK(unit_vector(v2)[0] == 0.0);
    CHECK(unit_vector(v2)[1] == 1.0);
    CHECK(unit_vector(v2)[2] == 0.0);
    CHECK(unit_vector(v3)[0] == 0.0);
    CHECK(unit_vector(v3)[1] == 0.0);
    CHECK(unit_vector(v3)[2] == 1.0);
    CHECK(unit_vector(v4)[0] ==
          doctest::Approx(+1.0 / std::sqrt(14.0)).epsilon(epsilon));
    CHECK(unit_vector(v4)[1] ==
          doctest::Approx(-2.0 / std::sqrt(14.0)).epsilon(epsilon));
    CHECK(unit_vector(v4)[2] ==
          doctest::Approx(+3.0 / std::sqrt(14.0)).epsilon(epsilon));
}

TEST_CASE("random_vec_vec3") {
    const auto min = -1.1;
    const auto max = 2.2;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        const Vec3 vec = random_vec<3_D>(min, max);
        CHECK_IN_RANGE(min, max, vec[0]);
        CHECK_IN_RANGE(min, max, vec[1]);
        CHECK_IN_RANGE(min, max, vec[2]);
    }
}

TEST_CASE("random_vec_inside_unit_sphere_vec2") {
    const auto reps = 1000;
    Vec2 avg{};
    for (auto i = 0; i < reps; ++i) {
        const Vec2 vec = random_vec_inside_unit_sphere<2_D>();
        avg += vec;
        CHECK_IN_RANGE(0.0, 1.0 + epsilon, length(vec));
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, length(avg));
}

TEST_CASE("random_vec_inside_unit_sphere_vec3") {
    const auto reps = 1000;
    Vec3 avg{};
    for (auto i = 0; i < reps; ++i) {
        const Vec3 vec = random_vec_inside_unit_sphere<3_D>();
        avg += vec;
        CHECK_IN_RANGE(0.0, 1.0 + epsilon, length(vec));
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, length(avg));
}

TEST_CASE("random_unit_vec_vec3") {
    const auto reps = 1000;
    Vec3 avg{};
    for (auto i = 0; i < reps; ++i) {
        const Vec3 vec = random_unit_vec<3_D>();
        avg += vec;
        CHECK(length(vec) == doctest::Approx(1.0).epsilon(epsilon));
    }
    avg *= 1.0 / reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, length(avg));
}

TEST_CASE("mat_vec_mult") {
    // row first
    const Mat3x3 mat{
        // clang-format off
        1.1, 2.2, 3.3,
        4.4, 5.5, 6.6,
        7.7, 8.8, 9.9
        // clang-format on
    };
    const Vec3 x{1.0, 0.0, 0.0};
    const Vec3 y{0.0, 1.0, 0.0};
    const Vec3 z{0.0, 0.0, 1.0};

    {
        // multiplication right
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, mat * x, (Vec3{1.1, 4.4, 7.7}));
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, mat * y, (Vec3{2.2, 5.5, 8.8}));
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, mat * z, (Vec3{3.3, 6.6, 9.9}));
    }
    {
        // multiplication left
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, x * mat, (Vec3{1.1, 2.2, 3.3}));
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, y * mat, (Vec3{4.4, 5.5, 6.6}));
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, z * mat, (Vec3{7.7, 8.8, 9.9}));
    }
}

TEST_CASE("mat3x3_rotation_mat") {
    const Vec3 axis{1.1, 2.2, 3.3};
    const Mat3x3 identity = Mat3x3{
        // clang-format off
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // clang-format on
    };

    const Mat3x3 mat = rotation_mat(axis);
    const Mat3x3 inv_mat = inverse_rotation_mat(axis);

    {
        // values
        const Mat3x3 expected{
            // clang-format off
            0.5811329478584188, -0.6399638097631108, -0.5027234021873948,
            0.09283351724552164, -0.5615787520127085, 0.8221990290456893,
            -0.8084964038195901, -0.524476527102342, -0.26694182424164375,
            // clang-format on
        };
        const Scalar epsilon = 1e-14;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, mat, expected);
    }
    {
        // values of inverse
        const Mat3x3 expected{
            // clang-format off
            0.5811329478584188, 0.09283351724552164, -0.8084964038195901,
            -0.6399638097631108, -0.5615787520127085, -0.524476527102342,
            -0.5027234021873948, 0.8221990290456893, -0.26694182424164375,
            // clang-format on
        };
        const Scalar epsilon = 1e-14;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, inv_mat, expected);
    }

    {
        // R * R^-1
        const Mat3x3 res = mat * inv_mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] ==
                      doctest::Approx(identity[i][j]).epsilon(epsilon));
            }
        }
    }
    {
        // R^-1 * R
        const Mat3x3 res = inv_mat * mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] ==
                      doctest::Approx(identity[i][j]).epsilon(epsilon));
            }
        }
    }
}

TEST_CASE("mat3x3_scaling_mat") {
    const Vec3 axis{1.1, 2.2, 3.3};
    const Mat3x3 identity = Mat3x3{
        // clang-format off
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0,
        // clang-format on
    };

    const Mat3x3 mat = scaling_mat<3_D>(axis);
    const Mat3x3 inv_mat = inverse_scaling_mat<3_D>(axis);

    {
        Mat3x3 res = mat;
        for (auto i = 0; i < 3; ++i) {
            res[i][i] -= axis[i];
        }
        CHECK(near_zero(res, epsilon));
    }
    {
        const Mat3x3 res = inv_mat * mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] ==
                      doctest::Approx(identity[i][j]).epsilon(epsilon));
            }
        }
    }
    {
        const Mat3x3 res = mat * inv_mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] ==
                      doctest::Approx(identity[i][j]).epsilon(epsilon));
            }
        }
    }
    {
        const Mat3x3 res = inv_mat * mat;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] ==
                      doctest::Approx(identity[i][j]).epsilon(epsilon));
            }
        }
    }
}

}}} // namespace cpp_raytracing::tensor::test
