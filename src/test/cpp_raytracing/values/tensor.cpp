#include "../../common.hpp"

#include <array>
#include <limits>

#include <cpp_raytracing/util.hpp>
#include <cpp_raytracing/values/tensor.hpp>

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1.e-12;

TEST_CASE("vec3_default_constructor") {
    const Vec3 vec{};
    CHECK(vec.x() == 0.0);
    CHECK(vec.y() == 0.0);
    CHECK(vec.z() == 0.0);
}

TEST_CASE("vec3_random_constructor") {
    const auto min = -1.1;
    const auto max = 2.2;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    for (long i = 0; i < 1000; ++i) {
        const Vec3 vec = Vec3::random(min, max);
        CHECK_IN_RANGE(min, max, vec.x());
        CHECK_IN_RANGE(min, max, vec.y());
        CHECK_IN_RANGE(min, max, vec.z());
    }
}

TEST_CASE("vec3_axes") {
    Vec3 vec{1.1, 2.2, 3.3};
    CHECK(vec.x() == 1.1);
    CHECK(vec.y() == 2.2);
    CHECK(vec.z() == 3.3);
}

TEST_CASE("vec3_iterator") {
    SUBCASE("mutable read") {
        Vec3 vec{1.1, 2.2, 3.3};
        auto it = vec.begin();
        CHECK(*it == 1.1);
        ++it;
        CHECK(*it == 2.2);
        ++it;
        CHECK(*it == 3.3);
        ++it;
    }
    SUBCASE("const read") {
        const Vec3 vec{1.1, 2.2, 3.3};
        auto it = vec.begin();
        CHECK(*it == 1.1);
        ++it;
        CHECK(*it == 2.2);
        ++it;
        CHECK(*it == 3.3);
        ++it;
    }
    SUBCASE("mutable write") {
        Vec3 vec{1.1, 2.2, 3.3};
        for (auto& e : vec) {
            e = 7.7;
        }
        const std::array<Scalar, 3> elems = {7.7, 7.7, 7.7};
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, vec, elems);
    }
}

TEST_CASE("vec3_comparison") {
    const Vec3 vec1{1.1, 2.2, 3.3};
    const Vec3 vec2{1.1, 2.2, 3.3};
    const Vec3 vec3{7.7, 2.2, 3.3};
    const Vec3 vec4{1.1, 7.7, 3.3};
    const Vec3 vec5{1.1, 2.2, 7.7};

    SUBCASE("equals") {
        CHECK(vec1 == vec1);
        CHECK(vec1 == vec2);
        CHECK_FALSE(vec1 == vec3);
        CHECK_FALSE(vec1 == vec4);
        CHECK_FALSE(vec1 == vec5);
    }
    SUBCASE("unequals") {
        CHECK_FALSE(vec1 != vec1);
        CHECK_FALSE(vec1 != vec2);
        CHECK(vec1 != vec3);
        CHECK(vec1 != vec4);
        CHECK(vec1 != vec5);
    }
}

TEST_CASE("vec3_arithmetic") {
    const Vec3 x1{1.1, 2.2, 3.3};
    const Vec3 x2{4.4, 5.5, 6.6};
    const Scalar f = 7.7;

    {
        const auto res = x1 + x2;
        CHECK(res[0] == doctest::Approx(5.5).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(7.7).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(9.9).epsilon(epsilon));
    }
    {
        auto res = x1;
        res += x2;
        CHECK(res[0] == doctest::Approx(5.5).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(7.7).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(9.9).epsilon(epsilon));
    }
    {
        const auto res = x2 - x1;
        CHECK(res[0] == doctest::Approx(3.3).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(3.3).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3).epsilon(epsilon));
    }
    {
        auto res = x2;
        res -= x1;
        CHECK(res[0] == doctest::Approx(3.3).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(3.3).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3).epsilon(epsilon));
    }
    {
        const auto res = x1 * f;
        CHECK(res[0] == doctest::Approx(1.1 * 7.7).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.2 * 7.7).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3 * 7.7).epsilon(epsilon));
    }
    {
        const auto res = f * x1;
        CHECK(res[0] == doctest::Approx(1.1 * 7.7).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.2 * 7.7).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3 * 7.7).epsilon(epsilon));
    }
    {
        auto res = x1;
        res *= f;
        CHECK(res[0] == doctest::Approx(1.1 * 7.7).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.2 * 7.7).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3 * 7.7).epsilon(epsilon));
    }
    {
        const auto res = x1 / f;
        CHECK(res[0] == doctest::Approx(1.0 / 7.0).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.0 / 7.0).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.0 / 7.0).epsilon(epsilon));
    }
    {
        auto res = x1;
        res /= f;
        CHECK(res[0] == doctest::Approx(1.0 / 7.0).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.0 / 7.0).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.0 / 7.0).epsilon(epsilon));
    }
    {
        const auto res = dot(x1, x2);
        CHECK(res == doctest::Approx(38.72).epsilon(epsilon));
    }
    {
        const auto res = cross(x1, x2);
        CHECK(res[0] ==
              doctest::Approx(2.2 * 6.6 - 3.3 * 5.5).epsilon(epsilon));
        CHECK(res[1] ==
              doctest::Approx(3.3 * 4.4 - 1.1 * 6.6).epsilon(epsilon));
        CHECK(res[2] ==
              doctest::Approx(1.1 * 5.5 - 2.2 * 4.4).epsilon(epsilon));
    }
    {
        const auto res = unit_vector(x1);
        const auto len = std::sqrt(1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3);
        CHECK(res[0] == doctest::Approx(1.1 / len).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.2 / len).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.3 / len).epsilon(epsilon));
    }
    {
        const auto res = x1.length();
        const auto len = std::sqrt(1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3);
        CHECK(res == doctest::Approx(len).epsilon(epsilon));
    }
    {
        const auto res = x1.length_squared();
        const auto len_sq = 1.1 * 1.1 + 2.2 * 2.2 + 3.3 * 3.3;
        CHECK(res == doctest::Approx(len_sq).epsilon(epsilon));
    }
}

TEST_CASE("vec3_near_zero") {
    CHECK(Vec3(1.0e-15, 1.0e-16, 1.0e-17).near_zero(epsilon));
    CHECK_FALSE(Vec3(1.0e-04, 1.0e-16, 1.0e-17).near_zero(epsilon));
    CHECK_FALSE(Vec3(1.0e-15, 1.0e-04, 1.0e-17).near_zero(epsilon));
    CHECK_FALSE(Vec3(1.0e-15, 1.0e-16, 1.0e-04).near_zero(epsilon));
}

TEST_CASE("vec3_random_in_unit_disk") {
    const auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_in_unit_disk();
        avg += vec;
        CHECK(vec.z() == 0.0);
        CHECK_IN_RANGE(0.0, 1.0 + epsilon, vec.length());
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, avg.length());
}

TEST_CASE("vec3_random_vector_in_unit_sphere") {
    const auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_vector_in_unit_sphere();
        avg += vec;
        CHECK_IN_RANGE(0.0, 1.0 + epsilon, vec.length());
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, avg.length());
}

TEST_CASE("vec3_random_unit_vector") {
    const auto reps = 1000;
    Vec3 avg{};
    for (long i = 0; i < reps; ++i) {
        const Vec3 vec = random_unit_vector();
        avg += vec;
        CHECK(vec.length() == doctest::Approx(1.0).epsilon(epsilon));
    }
    avg /= reps;
    // note: has small probability to generate a false-positive
    CHECK_IN_RANGE(0.0, 0.05, avg.length());
}

TEST_CASE("vec6_default_constructor") {
    Vec6 vec{};
    CHECK(vec.x() == 0.0);
    CHECK(vec.y() == 0.0);
    CHECK(vec.z() == 0.0);
    CHECK(vec.u() == 0.0);
    CHECK(vec.v() == 0.0);
    CHECK(vec.w() == 0.0);
}

TEST_CASE("vec6_axes") {
    Vec6 vec{1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    CHECK(vec.x() == 1.1);
    CHECK(vec.y() == 2.2);
    CHECK(vec.z() == 3.3);
    CHECK(vec.u() == 4.4);
    CHECK(vec.v() == 5.5);
    CHECK(vec.w() == 6.6);
}

TEST_CASE("vec6_comparison") {
    const Vec6 vec1{1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    const Vec6 vec2{1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    const Vec6 vec3{7.7, 2.2, 3.3, 4.4, 5.5, 6.6};
    const Vec6 vec4{1.1, 7.7, 3.3, 4.4, 5.5, 6.6};
    const Vec6 vec5{1.1, 2.2, 7.7, 4.4, 5.5, 6.6};
    const Vec6 vec6{1.1, 2.2, 3.3, 7.7, 5.5, 6.6};
    const Vec6 vec7{1.1, 2.2, 3.3, 4.4, 7.7, 6.6};
    const Vec6 vec8{1.1, 2.2, 3.3, 4.4, 5.5, 7.7};

    {
        CHECK(vec1 == vec1);
        CHECK(vec1 == vec2);
        CHECK_FALSE(vec1 == vec3);
        CHECK_FALSE(vec1 == vec4);
        CHECK_FALSE(vec1 == vec5);
        CHECK_FALSE(vec1 == vec6);
        CHECK_FALSE(vec1 == vec7);
        CHECK_FALSE(vec1 == vec8);
    }
    {
        CHECK_FALSE(vec1 != vec1);
        CHECK_FALSE(vec1 != vec2);
        CHECK(vec1 != vec3);
        CHECK(vec1 != vec4);
        CHECK(vec1 != vec5);
        CHECK(vec1 != vec6);
        CHECK(vec1 != vec7);
        CHECK(vec1 != vec8);
    }
}

TEST_CASE("vec6_arithmetic") {
    const Vec6 x1{1, 2, 3, 4, 5, 6};
    const Vec6 x2{7, 8, 9, 10, 11, 12};
    const Scalar f = 13;

    {
        const auto res = x1 + x2;
        CHECK(res[0] == doctest::Approx(8).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(10).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(12).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(14).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(16).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(18).epsilon(epsilon));
    }
    {
        auto res = x1;
        res += x2;
        CHECK(res[0] == doctest::Approx(8).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(10).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(12).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(14).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(16).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(18).epsilon(epsilon));
    }
    {
        const auto res = x2 - x1;
        CHECK(res[0] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(6).epsilon(epsilon));
    }
    {
        auto res = x2;
        res -= x1;
        CHECK(res[0] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(6).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(6).epsilon(epsilon));
    }
    {
        const auto res = x1 * f;
        CHECK(res[0] == doctest::Approx(13).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(26).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(39).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(52).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(65).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(78).epsilon(epsilon));
    }
    {
        const auto res = f * x1;
        CHECK(res[0] == doctest::Approx(13).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(26).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(39).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(52).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(65).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(78).epsilon(epsilon));
    }
    {
        auto res = x1;
        res *= f;
        CHECK(res[0] == doctest::Approx(13).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(26).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(39).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(52).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(65).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(78).epsilon(epsilon));
    }
    {
        const auto res = x1 / f;
        CHECK(res[0] == doctest::Approx(1.0 / 13.0).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.0 / 13.0).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.0 / 13.0).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(4.0 / 13.0).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(5.0 / 13.0).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(6.0 / 13.0).epsilon(epsilon));
    }
    {
        auto res = x1;
        res /= f;
        CHECK(res[0] == doctest::Approx(1.0 / 13.0).epsilon(epsilon));
        CHECK(res[1] == doctest::Approx(2.0 / 13.0).epsilon(epsilon));
        CHECK(res[2] == doctest::Approx(3.0 / 13.0).epsilon(epsilon));
        CHECK(res[3] == doctest::Approx(4.0 / 13.0).epsilon(epsilon));
        CHECK(res[4] == doctest::Approx(5.0 / 13.0).epsilon(epsilon));
        CHECK(res[5] == doctest::Approx(6.0 / 13.0).epsilon(epsilon));
    }
}

TEST_CASE("vec6_near_zero") {
    CHECK(Vec6(1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19, 1.0e-20)
              .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-4, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19, 1.0e-20)
                    .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-15, 1.0e-4, 1.0e-17, 1.0e-18, 1.0e-19, 1.0e-20)
                    .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-15, 1.0e-16, 1.0e-4, 1.0e-18, 1.0e-19, 1.0e-20)
                    .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-15, 1.0e-16, 1.0e-17, 1.0e-4, 1.0e-19, 1.0e-20)
                    .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-4, 1.0e-20)
                    .near_zero(epsilon));
    CHECK_FALSE(Vec6(1.0e-15, 1.0e-16, 1.0e-17, 1.0e-18, 1.0e-19, 1.0e-4)
                    .near_zero(epsilon));
}

TEST_CASE("mat3x3_default_constructor") {
    const Mat3x3 mat{};
    CHECK(mat.x() == Vec3{1.0, 0.0, 0.0});
    CHECK(mat.y() == Vec3{0.0, 1.0, 0.0});
    CHECK(mat.z() == Vec3{0.0, 0.0, 1.0});
}

TEST_CASE("mat3x3_convention") {
    // row first
    const Mat3x3 mat{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
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

TEST_CASE("mat3x3_comparison") {
    const Mat3x3 mat1{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    const Mat3x3 mat2{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    const Mat3x3 mat3{{1.1, 12.34, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    const Mat3x3 mat4{{1.1, 2.2, 3.3}, {12.34, 5.5, 6.6}, {7.7, 8.8, 9.9}};
    const Mat3x3 mat5{{1.1, 2.2, 3.3}, {4.4, 5.5, 6.6}, {7.7, 8.8, 12.34}};

    {
        CHECK(mat1 == mat1);
        CHECK(mat1 == mat2);
        CHECK_FALSE(mat1 == mat3);
        CHECK_FALSE(mat1 == mat4);
        CHECK_FALSE(mat1 == mat5);
    }
    {
        CHECK_FALSE(mat1 != mat1);
        CHECK_FALSE(mat1 != mat2);
        CHECK(mat1 != mat3);
        CHECK(mat1 != mat4);
        CHECK(mat1 != mat5);
    }
}

TEST_CASE("mat3x3_arithmetic") {
    const Mat3x3 mat1{{1.9, 2.8, 3.7}, {4.6, 5.5, 6.4}, {3.7, 2.8, 1.9}};
    const Mat3x3 mat2{{9.1, 8.2, 7.3}, {6.4, 5.5, 4.6}, {3.7, 2.8, 1.9}};
    const Scalar f = 1 / 11.0;

    {
        const auto res = mat1 + mat2;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(11.0).epsilon(epsilon));
            }
        }
    }
    {
        auto res = mat1;
        res += mat2;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(11.0).epsilon(epsilon));
            }
        }
    }
    {
        const auto res = mat1 - mat1;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(0.0).epsilon(epsilon));
            }
        }
    }
    {
        auto res = mat1;
        res -= mat1;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(0.0).epsilon(epsilon));
            }
        }
    }
    {
        const auto res = (mat1 + mat2) * f;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(1.0).epsilon(epsilon));
            }
        }
    }
    {
        const auto res = f * (mat1 + mat2);
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(1.0).epsilon(epsilon));
            }
        }
    }
    {
        auto res = (mat1 + mat2);
        res *= f;
        for (auto i = 0; i < 3; ++i) {
            for (auto j = 0; i < 3; ++i) {
                CHECK(res[i][j] == doctest::Approx(1.0).epsilon(epsilon));
            }
        }
    }
}

TEST_CASE("mat3x3_rotation_mat") {
    const Vec3 axis{1.1, 2.2, 3.3};
    const Mat3x3 identity = Mat3x3{
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
    };

    const Mat3x3 mat = rotation_mat(axis);
    const Mat3x3 inv_mat = inverse_rotation_mat(axis);

    {
        // values
        const Mat3x3 expected{
            {0.5811329478584188, -0.6399638097631108, -0.5027234021873948},
            {0.09283351724552164, -0.5615787520127085, 0.8221990290456893},
            {-0.8084964038195901, -0.524476527102342, -0.26694182424164375}};
        const Scalar epsilon = 1e-14;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, mat, expected);
    }
    {
        // values of inverse
        const Mat3x3 expected{
            {0.5811329478584188, 0.09283351724552164, -0.8084964038195901},
            {-0.6399638097631108, -0.5615787520127085, -0.524476527102342},
            {-0.5027234021873948, 0.8221990290456893, -0.26694182424164375}};
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

}} // namespace cpp_raytracing::test
