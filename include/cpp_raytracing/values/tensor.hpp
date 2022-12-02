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

/** @brief returns tuple of two vectors from first and second half of vector */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS, typename Traits>
constexpr std::tuple<
    gttl::Tensor<Scalar, 1, gttl::Dimensions<1>{DIMENSIONS[0] / 2}, Traits>,
    gttl::Tensor<Scalar, 1, gttl::Dimensions<1>{DIMENSIONS[0] / 2}, Traits>>
split(const gttl::Tensor<Scalar, 1, DIMENSIONS, Traits>& vec) requires(
    DIMENSIONS[0] % 2 == 0) {

    constexpr Dimension HALF = DIMENSIONS[0] / 2;
    using HalfTensor =
        gttl::Tensor<Scalar, 1, gttl::Dimensions<1>{HALF}, Traits>;

    std::tuple<HalfTensor, HalfTensor> res{};

    for (Dimension i{0}; i < HALF; ++i) {
        std::get<0>(res)[i] = vec[i];
    }
    for (Dimension i{0}; i < HALF; ++i) {
        std::get<1>(res)[i] = vec[i + HALF];
    }

    return res;
}

/** @brief returns joined vectors */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS1,
          gttl::Dimensions<1> DIMENSIONS2, typename Traits>
constexpr gttl::Tensor<
    Scalar, 1, gttl::Dimensions<1>{DIMENSIONS1[0] + DIMENSIONS2[0]}, Traits>
outer_sum(const gttl::Tensor<Scalar, 1, DIMENSIONS1, Traits>& vec1,
          const gttl::Tensor<Scalar, 1, DIMENSIONS2, Traits>& vec2) {
    using SumTensor =
        gttl::Tensor<Scalar, 1,
                     gttl::Dimensions<1>{DIMENSIONS1[0] + DIMENSIONS2[0]},
                     Traits>;
    SumTensor res{};

    for (Dimension i{0}; i < DIMENSIONS1[0]; ++i) {
        res[i] = vec1[i];
    }
    for (Dimension i{0}; i < DIMENSIONS2[0]; ++i) {
        res[DIMENSIONS1[0] + i] = vec2[i];
    }

    return res;
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

/** @brief unit/canonical basis vector */
template <Dimension DIMENSION, std::size_t N>
requires(N < DIMENSION) constexpr Vec<DIMENSION> base_vec = []() constexpr {
    Vec<DIMENSION> vec{};
    vec.coefficients[N] = 1;
    return vec;
}
();

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

/**
 * @brief returns coordinates of x projected into the subspace spanned by two
 *        basis vectors at the origin
 * @note `x = u * basis_vec1 + v * basis_vec2 + w * normal` where `w` is unknown
 *        and `normal` is the plane's normal vector.
 * @note Can be used to calculate the u-v coordinates of a point in a plane.
 * @note The result is undefined if the basis vectors are parallel.
 */
template <Dimension DIMENSION>
constexpr Vec<Dimension{2}>
get_coords_in_plane(const Vec<DIMENSION>& basis_vec1,
                    const Vec<DIMENSION>& basis_vec2, const Vec<DIMENSION>& x) {
    using namespace tensor;

    const Vec<DIMENSION> b1 = basis_vec1;
    const Vec<DIMENSION> b2 = basis_vec2;

    const Scalar b1b1 = dot(b1, b1);
    const Scalar b1b2 = dot(b1, b2);
    const Scalar b2b2 = dot(b2, b2);
    const Scalar D = b1b1 * b2b2 - b1b2 * b1b2;
    const Scalar b1x = dot(b1, x);
    const Scalar b2x = dot(b2, x);
    const Scalar u = (b2b2 * b1x - b1b2 * b2x) / D;
    const Scalar v = (b1b1 * b2x - b1b2 * b1x) / D;
    return {u, v};
}

/** @brief matrix-vector multiplication */
template <typename Scalar, gttl::Dimensions<2> DIMENSIONS_MAT,
          gttl::Dimensions<1> DIMENSIONS_VEC, typename Traits>
constexpr auto
operator*(const gttl::Tensor<Scalar, 2, DIMENSIONS_MAT, Traits>& mat,
          const gttl::Tensor<Scalar, 1, DIMENSIONS_VEC, Traits>& vec) {
    return gttl::contraction<1, 2>(mat, vec);
}

/** @brief vector-matrix multiplication */
template <typename Scalar, gttl::Dimensions<1> DIMENSIONS_VEC,
          gttl::Dimensions<2> DIMENSIONS_MAT, typename Traits>
constexpr auto
operator*(const gttl::Tensor<Scalar, 1, DIMENSIONS_VEC, Traits>& vec,
          const gttl::Tensor<Scalar, 2, DIMENSIONS_MAT, Traits>& mat) {
    return gttl::contraction<0, 1>(vec, mat);
}

/** @brief matrix-matrix multiplication */
template <typename Scalar, gttl::Dimensions<2> DIMENSIONS1,
          gttl::Dimensions<2> DIMENSIONS2, typename Traits>
constexpr auto
operator*(const gttl::Tensor<Scalar, 2, DIMENSIONS1, Traits>& lhs,
          const gttl::Tensor<Scalar, 2, DIMENSIONS2, Traits>& rhs) {
    return gttl::contraction<1, 2>(lhs, rhs);
}

/**
 * @brief returns a vector with the subvector embeded and zero elsewere
 * @see projected_vector
 */
template <Dimension DIMENSION, Dimension POSITION, Dimension SUBDIMENSION>
constexpr Vec<DIMENSION>
embeded_vector(const Vec<SUBDIMENSION>& subvector) requires(
    (DIMENSION >= SUBDIMENSION) && (POSITION <= DIMENSION - SUBDIMENSION)) {
    // zero-initialization is required
    Vec<DIMENSION> vec{};
    for (Dimension i{0}; i < SUBDIMENSION; ++i) {
        vec.coefficients[i + POSITION] = subvector.coefficients[i];
    }
    return vec;
}

/**
 * @brief returns a subvector projected from the vector
 * @see embeded_vector
 */
template <Dimension SUBDIMENSION, Dimension POSITION, Dimension DIMENSION>
constexpr Vec<SUBDIMENSION>
projected_vector(const Vec<DIMENSION>& vector) requires(
    (DIMENSION >= SUBDIMENSION) && (POSITION <= DIMENSION - SUBDIMENSION)) {
    // zero-initialization is required
    Vec<SUBDIMENSION> subvec{};
    for (Dimension i{0}; i < SUBDIMENSION; ++i) {
        subvec.coefficients[i] = vector.coefficients[i + POSITION];
    }
    return subvec;
}

/**
 * @brief returns a matrix with the submatrix embeded and zero elsewere
 */
template <Dimension MAT_DIMENSION0, Dimension MAT_DIMENSION1,
          Dimension POSITION0, Dimension POSITION1, Dimension SUBMAT_DIMENSION0,
          Dimension SUBMAT_DIMENSION1>
inline Mat<MAT_DIMENSION0, MAT_DIMENSION1> embeded_matrix(
    const Mat<SUBMAT_DIMENSION0, SUBMAT_DIMENSION1>&
        submatrix) requires((MAT_DIMENSION0 >= SUBMAT_DIMENSION0) &&
                            (MAT_DIMENSION1 >= SUBMAT_DIMENSION1) &&
                            (POSITION0 <= MAT_DIMENSION0 - SUBMAT_DIMENSION0) &&
                            (POSITION1 <= MAT_DIMENSION1 - SUBMAT_DIMENSION1)) {
    // zero-initialization is required
    Mat<MAT_DIMENSION0, MAT_DIMENSION1> mat{};
    for (Dimension i{0}; i < SUBMAT_DIMENSION0; ++i) {
        for (Dimension j{0}; j < SUBMAT_DIMENSION1; ++j) {
            mat[i + POSITION0][j + POSITION1] = submatrix[i][j];
        }
    }
    return mat;
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
