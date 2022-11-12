/**
 * @file
 * @brief tensors (linear math)
 */

#ifndef CPP_RAYTRACING_MATH_HPP
#define CPP_RAYTRACING_MATH_HPP

#include <array>
#include <cmath>
#include <utility>

#include <gttl/dimensions.hpp>
#include <gttl/tensor.hpp>

#include "random.hpp"
#include "scalar.hpp"

namespace cpp_raytracing {

using gttl::literals::operator"" _D;

/**
 * @brief dimension of vector space
 * @note forward alias from gttl for dependecy inversion
 */
using Dimension = gttl::Dimension;

namespace literals {

/**
 * @brief literal suffix for Dimension
 * @note forward alias from gttl for dependecy inversion
 */
constexpr Dimension operator"" _D(unsigned long long value) {
    return static_cast<Dimension>(value);
}

} // namespace literals

/**
 * @brief dimensions of tensor space
 * @tparam RANK amount of vector spaces constituting the tensor product space
 * @note forward alias from gttl for dependecy inversion
 */
template <std::size_t RANK>
using Dimensions = gttl::Dimensions<RANK>;

/** @brief generic vector */
template <Dimension DIMENSION>
using Vec = gttl::Tensor<Scalar, 1, Dimensions<1>{DIMENSION}>;

/** @brief 2D vector */
using Vec2 = Vec<2_D>;
/** @brief 3D vector */
using Vec3 = Vec<3_D>;
/** @brief 6D vector */
using Vec6 = Vec<6_D>;

/** @brief generic matrix */
template <Dimension DIMENSION1, Dimension DIMENSION2 = DIMENSION1>
using Mat = gttl::Tensor<Scalar, 2, Dimensions<2>{DIMENSION1, DIMENSION2}>;

/** @brief 3x3 matrix */
using Mat3x3 = Mat<3_D>;

/** @brief generic (Christoffel's symbols-like) rank3 tensor  */
template <Dimension DIMENSION>
using TenR3 =
    gttl::Tensor<Scalar, 3, Dimensions<3>{DIMENSION, DIMENSION, DIMENSION}>;

/** @brief 3x3x3 rank3 tensor */
using Ten3x3x3 = TenR3<3_D>;

namespace tensor {

/** @brief true, iff sum of absolutes of elements is exactly zero */
template <typename Tensor>
constexpr bool is_zero(const Tensor& ten) {
    const auto abs = typename Tensor::traits_type::abs{};
    Scalar total = 0;
    for (const auto& x : ten) {
        total += abs(x);
    }
    return abs(total) == Scalar{0};
}

/** @brief true, iff sum of absolutes of elements is smaller than epsilon */
template <typename Tensor>
constexpr bool near_zero(const Tensor& ten, const Scalar epsilon) {
    const auto abs = typename Tensor::traits_type::abs{};
    Scalar total = 0;
    for (const auto& x : ten) {
        total += abs(x);
    }
    return abs(total) < epsilon;
}

/** @brief returns tuple of two 3D vectors from 6D vector */
constexpr std::tuple<Vec3, Vec3> split(const Vec6& vec) {
    return {
        Vec3{vec[0], vec[1], vec[2]},
        Vec3{vec[3], vec[4], vec[5]},
    };
}

/** @brief returns 6D vector from two 3D vectors */
constexpr Vec6 outer_sum(const Vec3& lhs, const Vec3& rhs) {
    return {
        lhs[0], lhs[1], lhs[2], rhs[0], rhs[1], rhs[2],
    };
}

/** @brief returns dot product of vectors */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr Scalar dot(const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& lhs,
                     const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& rhs) {
    return gttl::contraction<0, 1>(lhs, rhs);
}

/** @brief returns cross product of 3D vectors */
constexpr Vec3 cross(const Vec3& lhs, const Vec3& rhs) {
    return Vec3{
        lhs[1] * rhs[2] - lhs[2] * rhs[1],
        lhs[2] * rhs[0] - lhs[0] * rhs[2],
        lhs[0] * rhs[1] - lhs[1] * rhs[0],
    };
}

/** @brief returns square of length of vector (= rank1 tensor) */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr Scalar
length_squared(const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec) {
    return dot(vec, vec);
}

/** @brief returns square of length of vector (= rank1 tensor) */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr Scalar
length(const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec) {
    return std::sqrt(length_squared(vec));
}

/** @brief returns vector of length one */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>
unit_vector(const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec) {
    return vec * (1.0 / length(vec));
}

/** @brief zero vector */
template <Dimension DIMENSION>
constexpr Vec<DIMENSION> zero_vec{};

/** @brief random vector where each element is in the given range */
template <Dimension DIMENSION>
inline Vec<DIMENSION> random_vec(const Scalar min, const Scalar max) {
    Vec<DIMENSION> vec;
    for (auto& x : vec) {
        x = random_scalar(min, max);
    }
    return vec;
}

/** @brief random vector where each element is in the given range */
template <Dimension DIMENSION>
inline Vec<DIMENSION> random_unit_vec(const Scalar min, const Scalar max) {
    Vec<DIMENSION> vec;
    for (auto& x : vec) {
        x = random_scalar(min, max);
    }
    return vec;
}

/** @brief random vector of at most unit length (uniform distribution) */
template <Dimension DIMENSION>
inline Vec<DIMENSION> random_vec_inside_unit_sphere() {
    // rejection based and uniform
    // note: inefficient for large dimensions but okay for <=3D
    while (true) {
        const auto vec = random_vec<DIMENSION>(-1.0, 1.0);
        if (length_squared(vec) >= 1.0)
            continue;
        return vec;
    }
}

/** @brief random vector on surface of unit sphere (uniform distribution) */
template <Dimension DIMENSION>
inline Vec<DIMENSION> random_unit_vec() {
    return unit_vector(random_vec_inside_unit_sphere<DIMENSION>());
}

/** @brief matrix-vector multiplication */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr gttl::Tensor<Scalar, 1, DIMENSIONS, Traits> operator*(
    const gttl::Tensor<Scalar, 2,
                       gttl::cexpr::array::concatenate(DIMENSIONS, DIMENSIONS),
                       Traits>& mat,
    const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec) {
    return gttl::contraction<1, 2>(mat, vec);
}

/** @brief vector-matrix multiplication */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr gttl::Tensor<Scalar, 1, DIMENSIONS, Traits> operator*(
    const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec,
    const gttl::Tensor<Scalar, 2,
                       gttl::cexpr::array::concatenate(DIMENSIONS, DIMENSIONS),
                       Traits>& mat) {
    return gttl::contraction<0, 1>(vec, mat);
}

/** @brief matrix-matrix multiplication */
template <typename Scalar, std::size_t RANK1, std::size_t RANK2,
          gttl::Dimensions<RANK1> DIMENSIONS1,
          gttl::Dimensions<RANK2> DIMENSIONS2, typename Traits>
constexpr auto
operator*(const gttl::Tensor<Scalar, RANK1, DIMENSIONS1, Traits>& lhs,
          const gttl::Tensor<Scalar, RANK2, DIMENSIONS2, Traits>&
              rhs) requires((RANK1 == 2) && (RANK2 == 2)) {
    return gttl::contraction<1, 2>(lhs, rhs);
}

/**
 * @brief returns rotational matrix for given axis and amplitude
 * @note Rotations are performed around each axis separately in the order:
 *       `R_x`, `R_y`, `R_z` (roll, pitch, yaw, Trait-Bryan angles).
 * @see inverse_rotation_mat
 */
inline Mat3x3 rotation_mat(const Vec3& axis) {
    const Scalar cx = std::cos(axis[0]);
    const Scalar sx = std::sin(axis[0]);
    const Scalar cy = std::cos(axis[1]);
    const Scalar sy = std::sin(axis[1]);
    const Scalar cz = std::cos(axis[2]);
    const Scalar sz = std::sin(axis[2]);

    return Mat3x3{
        // clang-format off
        cy * cz, sx * sy * cz - cx * sz, cx * sy * cz + sx * sz,
        cy * sz, sx * sy * sz + cx * cz, cx * sy * sz - sx * cz,
        -sy, sx * cy, cx * cy,
        // clang-format on
    };
}

/**
 * @brief returns inverse rotational matrix for given axis and amplitude
 * @note Rotations are performed around each inverted axis separately in the
 *       order: `R_(-z)`, `R_(-y)`, `R_(-x)` (yaw, pitch, roll, Trait-Bryan
 * angles).
 * @see rotation_mat
 */
inline Mat3x3 inverse_rotation_mat(const Vec3& axis) {
    const Scalar cx = std::cos(axis[0]);
    const Scalar sx = std::sin(axis[0]);
    const Scalar cy = std::cos(axis[1]);
    const Scalar sy = std::sin(axis[1]);
    const Scalar cz = std::cos(axis[2]);
    const Scalar sz = std::sin(axis[2]);

    return Mat3x3{
        // clang-format off
        cy * cz, cy * sz, -sy,
        sx * sy * cz - cx * sz, cx * cz + sx * sy * sz, sx * cy,
        cx * sy * cz + sx * sz, -cz * sx + cx * sy * sz, cx * cy,
        // clang-format on
    };
}

/**
 * @brief returns identity matrix
 */
template <Dimension DIMENSION>
inline Mat<DIMENSION> make_identity_mat() {
    Mat<DIMENSION> mat{}; // zero-initialization is required
    for (std::size_t i = 0; i < DIMENSION; ++i) {
        mat[i][i] = Scalar{1};
    }
    return mat;
}

/**
 * @brief returns identity matrix
 */
template <Dimension DIMENSION>
const Mat<DIMENSION> identity_mat = make_identity_mat<DIMENSION>();

/**
 * @brief returns scaling matrix for given scale coefficients
 * @see inverse_scaling_mat
 */
template <Dimension DIMENSION>
constexpr Mat<DIMENSION> scaling_mat(const Vec<DIMENSION>& vec) {
    Mat<DIMENSION> mat{}; // zero-initialization is required
    for (std::size_t i = 0; i < DIMENSION; ++i) {
        mat[i][i] = vec[i];
    }
    return mat;
}

/**
 * @brief returns inverse scaling matrix for given scale coefficients
 * @see scaling_mat
 */
template <Dimension DIMENSION>
constexpr Mat<DIMENSION> inverse_scaling_mat(const Vec<DIMENSION>& vec) {
    Mat<DIMENSION> mat{}; // zero-initialization is required
    for (std::size_t i = 0; i < DIMENSION; ++i) {
        mat[i][i] = Scalar{1} / vec[i];
    }
    return mat;
}

} // namespace tensor
} // namespace cpp_raytracing
#endif
