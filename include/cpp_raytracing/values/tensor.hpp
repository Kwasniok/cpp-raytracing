/**
 * @file
 * @brief tensors (linear math)
 */

#ifndef CPP_RAYTRACING_MATH_HPP
#define CPP_RAYTRACING_MATH_HPP

#include <cmath>
#include <iostream>

#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../values/random.hpp"
#include "scalar.hpp"

namespace cpp_raytracing {

class Vec2;
class Vec3;
class Mat3x3;

/**
 * @brief 2D floating-point Vector
 */
class Vec2 {
  public:
    /** @brief initialize as zero vector */
    constexpr Vec2() : _data{0, 0} {}
    /** @brief initialize with coefficients */
    constexpr Vec2(const Scalar u, const Scalar v) : _data{u, v} {}

    /** @brief initialize coefficients with random values */
    inline static Vec2 random(Scalar min, Scalar max) {
        return {random_scalar(min, max), random_scalar(min, max)};
    }

    /** @brief get u coefficient */
    constexpr Scalar u() const { return _data.x; }
    /** @brief get v coefficient */
    constexpr Scalar v() const { return _data.y; }

    /** @brief const iterator for first element */
    constexpr auto begin() const { return &_data[0]; }
    /** @brief const iterator for end */
    constexpr auto end() const { return &_data[0] + 2; }
    /** @brief iterator for first element */
    constexpr auto begin() { return &_data[0]; }
    /** @brief iterator for end */
    constexpr auto end() { return &_data[0] + 2; }

    /** @brief tests equivalence */
    constexpr bool operator==(const Vec2& other) const {
        return _data == other._data;
    }

    /** @brief tests inequivalence */
    constexpr bool operator!=(const Vec2& other) const {
        return _data != other._data;
    }

    /** @brief negate elementwise */
    constexpr Vec2 operator-() const { return Vec2{-_data}; }

    /**
     * @brief enumerated accces to coefficients
     * @note: `0=u, 1=v`
     */
    constexpr Scalar operator[](unsigned long i) const { return _data[i]; }
    /**
     * @brief enumerated accces to coefficients
     * @note: `0=u, 1=v`
     */
    constexpr Scalar& operator[](unsigned long i) { return _data[i]; }

    /** @brief add elementwise */
    constexpr Vec2& operator+=(const Vec2& other) {
        _data += other._data;
        return *this;
    }
    /** @brief subtract elementwise */
    constexpr Vec2& operator-=(const Vec2& other) {
        _data -= other._data;
        return *this;
    }
    /** @brief multiply elementwise */
    constexpr Vec2& operator*=(const Scalar fac) {
        _data *= fac;
        return *this;
    }
    /** @brief divide elementwise */
    constexpr Vec2& operator/=(const Scalar fac) {
        _data /= fac;
        return *this;
    }

    /** @brief get length of the vector */
    constexpr Scalar length() const {
        // NOTE: for version 0.9.9 glm::length is not constexpr
        return sqrt(length_squared());
    }

    /**
     * @brief get length of the vector squared
     * @note Fater than std::pow(length(),2)`
     */
    constexpr Scalar length_squared() const { return glm::dot(_data, _data); }

    /** @brief tests if vector is zero vector */
    constexpr bool near_zero(const Scalar epsilon) const {
        return std::abs(_data.x) < epsilon && std::abs(_data.y) < epsilon;
    }

    /** @brief zero vector */
    static constexpr Vec2 zero() { return Vec2{}; }

    friend constexpr Vec2 operator+(const Vec2&, const Vec2&);
    friend constexpr Vec2 operator-(const Vec2&, const Vec2&);
    friend constexpr Vec2 operator*(const Vec2&, const Scalar);
    friend constexpr Vec2 operator*(const Scalar, const Vec2&);
    friend constexpr Vec2 operator/(const Vec2&, const Scalar);
    friend constexpr Scalar dot(const Vec2&, const Vec2&);
    friend constexpr Vec2 unit_vector(const Vec2&);

  private:
    using data_type = glm::vec<2, Scalar>;
    constexpr Vec2(const data_type& data) : _data(data) {}
    constexpr Vec2(data_type&& data) : _data(std::move(data)) {}

    data_type _data;
};

/** @brief write vector as space separated components */
inline std::ostream& operator<<(std::ostream& os, const Vec2 v) {
    os << "Vec2(" << v[0] << ", " << v[1] << ")";
    return os;
}

/** @brief add elementwise */
inline constexpr Vec2 operator+(const Vec2& v1, const Vec2& v2) {
    return Vec2(v1._data + v2._data);
}

/** @brief subtract elementwise */
inline constexpr Vec2 operator-(const Vec2& v1, const Vec2& v2) {
    return Vec2(v1._data - v2._data);
}

/** @brief multiply elementwise */
inline constexpr Vec2 operator*(const Vec2& v, const Scalar f) {
    return Vec2(v._data * f);
}

/** @brief multiply elementwise */
inline constexpr Vec2 operator*(const Scalar f, const Vec2& v) {
    return v * f;
}

/** @brief divide elementwise */
inline constexpr Vec2 operator/(const Vec2& v, const Scalar f) {
    return Vec2(v._data / f);
}

/** @brief scalar product (inner product) */
inline constexpr Scalar dot(const Vec2& v1, const Vec2& v2) {
    return glm::dot(v1._data, v2._data);
}

/** @brief get normalized vector */
inline constexpr Vec2 unit_vector(const Vec2& v) {
    return v / v.length();
}

/**
 * @brief 3D floating-point Vector
 */
class Vec3 {
  public:
    /** @brief initialize as zero vector */
    constexpr Vec3() : _data{0, 0, 0} {}
    /** @brief initialize with coefficients */
    constexpr Vec3(const Scalar x, const Scalar y, const Scalar z)
        : _data{x, y, z} {}

    /** @brief initialize coefficients with random values */
    inline static Vec3 random(Scalar min, Scalar max) {
        return {random_scalar(min, max), random_scalar(min, max),
                random_scalar(min, max)};
    }

    /** @brief get x coefficient */
    constexpr Scalar x() const { return _data.x; }
    /** @brief get y coefficient */
    constexpr Scalar y() const { return _data.y; }
    /** @brief get z coefficient */
    constexpr Scalar z() const { return _data.z; }

    /** @brief const iterator for first element */
    constexpr auto begin() const { return &_data[0]; }
    /** @brief const iterator for end */
    constexpr auto end() const { return &_data[0] + 3; }
    /** @brief iterator for first element */
    constexpr auto begin() { return &_data[0]; }
    /** @brief iterator for end */
    constexpr auto end() { return &_data[0] + 3; }

    /** @brief tests equivalence */
    constexpr bool operator==(const Vec3& other) const {
        return _data == other._data;
    }

    /** @brief tests inequivalence */
    constexpr bool operator!=(const Vec3& other) const {
        return _data != other._data;
    }

    /** @brief negate elementwise */
    constexpr Vec3 operator-() const { return Vec3{-_data}; }

    /**
     * @brief enumerated accces to coefficients
     * @note: `0=x, 1=y, 2=z`
     */
    constexpr Scalar operator[](unsigned long i) const { return _data[i]; }
    /**
     * @brief enumerated accces to coefficients
     * @note: `0=x, 1=y, 2=z`
     */
    constexpr Scalar& operator[](unsigned long i) { return _data[i]; }

    /** @brief add elementwise */
    constexpr Vec3& operator+=(const Vec3& other) {
        _data += other._data;
        return *this;
    }
    /** @brief subtract elementwise */
    constexpr Vec3& operator-=(const Vec3& other) {
        _data -= other._data;
        return *this;
    }
    /** @brief multiply elementwise */
    constexpr Vec3& operator*=(const Scalar fac) {
        _data *= fac;
        return *this;
    }
    /** @brief divide elementwise */
    constexpr Vec3& operator/=(const Scalar fac) {
        _data /= fac;
        return *this;
    }

    /** @brief get length of the vector */
    constexpr Scalar length() const {
        // NOTE: for version 0.9.9 glm::length is not constexpr
        return sqrt(length_squared());
    }

    /**
     * @brief get length of the vector squared
     * @note Fater than std::pow(length(),2)`
     */
    constexpr Scalar length_squared() const { return glm::dot(_data, _data); }

    /** @brief tests if vector is zero vector */
    constexpr bool near_zero(const Scalar epsilon) const {
        return std::abs(_data.x) < epsilon && std::abs(_data.y) < epsilon &&
               std::abs(_data.z) < epsilon;
    }

    /** @brief zero vector */
    static constexpr Vec3 zero() { return Vec3{}; }

    friend constexpr Vec3 operator+(const Vec3&, const Vec3&);
    friend constexpr Vec3 operator-(const Vec3&, const Vec3&);
    friend constexpr Vec3 operator*(const Vec3&, const Scalar);
    friend constexpr Vec3 operator*(const Scalar, const Vec3&);
    friend constexpr Vec3 operator/(const Vec3&, const Scalar);
    friend constexpr Scalar dot(const Vec3&, const Vec3&);
    friend constexpr Vec3 cross(const Vec3&, const Vec3&);
    friend constexpr Vec3 unit_vector(const Vec3&);

    friend class Mat3x3;
    friend Vec3 operator*(const Mat3x3&, const Vec3&);
    friend Vec3 operator*(const Vec3&, const Mat3x3&);

  private:
    using data_type = glm::vec<3, Scalar>;
    constexpr Vec3(const data_type& data) : _data(data) {}
    constexpr Vec3(data_type&& data) : _data(std::move(data)) {}

    data_type _data;
};

/** @brief write vector as space separated components */
inline std::ostream& operator<<(std::ostream& os, const Vec3 v) {
    os << "Vec3(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return os;
}

/** @brief add elementwise */
inline constexpr Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1._data + v2._data);
}

/** @brief subtract elementwise */
inline constexpr Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1._data - v2._data);
}

/** @brief multiply elementwise */
inline constexpr Vec3 operator*(const Vec3& v, const Scalar f) {
    return Vec3(v._data * f);
}

/** @brief multiply elementwise */
inline constexpr Vec3 operator*(const Scalar f, const Vec3& v) {
    return v * f;
}

/** @brief divide elementwise */
inline constexpr Vec3 operator/(const Vec3& v, const Scalar f) {
    return Vec3(v._data / f);
}

/** @brief scalar product (inner product) */
inline constexpr Scalar dot(const Vec3& v1, const Vec3& v2) {
    return glm::dot(v1._data, v2._data);
}

/** @brief cross product (vector product) */
inline constexpr Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3(glm::cross(v1._data, v2._data));
}

/** @brief get normalized vector */
inline constexpr Vec3 unit_vector(const Vec3& v) {
    return v / v.length();
}

/** @brief random vector in 2D x-y-unit disk */
inline Vec3 random_in_unit_disk() {
    // rejection based and uniform
    while (true) {
        auto v =
            Vec3(random_scalar(-1.0, +1.0), random_scalar(-1.0, +1.0), 0.0);
        if (v.length_squared() >= 1)
            continue;
        return v;
    }
}

/** @brief random vector in 3D unit sphere */
inline Vec3 random_vector_in_unit_sphere() {
    // rejection based and uniform
    while (true) {
        const auto v = Vec3::random(-1.0, +1.0);
        if (dot(v, v) >= 1.0) {
            continue;
        }
        return v;
    }
}

/** @brief random vector in 2D surface of unit sphere */
inline Vec3 random_unit_vector() {
    return unit_vector(random_vector_in_unit_sphere());
}

// note: Some of the functions are laking constexpr because this was not
//      supported by GLM v0.9.9 at the time of writing.

/**
 * @brief 3Dx3D floating-point Matrix
 */
class Mat3x3 {
  public:
    /**
     * @brief initialize as identity matrix
     * @note Coefficients are listed row first.
     */
    constexpr Mat3x3() : _data{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0} {}
    /**
     *  @brief initialize with vectors
     * @note Each vector is a row.
     */
    constexpr Mat3x3(const Vec3& x, const Vec3& y, const Vec3& z)
        : _data{
              // note: glm stortes column first!
              x._data[0], y._data[0], z._data[0], x._data[1], y._data[1],
              z._data[1], x._data[2], y._data[2], z._data[2],
          } {}

    /** @brief get x coefficient */
    constexpr Vec3 x() const {
        // note: glm stortes column first!
        return Vec3{_data[0][0], _data[1][0], _data[2][0]};
    }
    /** @brief get y coefficient */
    constexpr Vec3 y() const {
        // note: glm stortes column first!
        return Vec3{_data[0][1], _data[1][1], _data[2][1]};
    }
    /** @brief get z coefficient */
    constexpr Vec3 z() const {
        // note: glm stortes column first!
        return Vec3{_data[0][2], _data[1][2], _data[2][2]};
    }

    /** @brief const iterator for first element */
    constexpr auto begin() const { return &_data[0][0]; }
    /** @brief const iterator for end */
    constexpr auto end() const { return &_data[0][0] + 9; }
    /** @brief iterator for first element */
    auto begin() { return &_data[0][0]; }
    /** @brief iterator for end */
    auto end() { return &_data[0][0] + 9; }

    /** @brief tests equivalence */
    constexpr bool operator==(const Mat3x3& other) const {
        return _data == other._data;
    }

    /** @brief tests inequivalence */
    bool operator!=(const Mat3x3& other) const { return _data != other._data; }

    /**
     * @brief enumerated accces to coefficients
     * @note: `0=x, 1=y, 2=z`
     */
    constexpr Vec3 operator[](unsigned long i) const { return Vec3{_data[i]}; }

    /** @brief negate elementwise */
    Mat3x3 operator-() const { return Mat3x3{-_data}; }

    /** @brief add elementwise */
    Mat3x3& operator+=(const Mat3x3& other) {
        _data += other._data;
        return *this;
    }
    /** @brief subtract elementwise */
    Mat3x3& operator-=(const Mat3x3& other) {
        _data -= other._data;
        return *this;
    }
    /** @brief matrix multiplication */
    Mat3x3& operator*=(const Mat3x3& other) {
        _data *= other._data;
        return *this;
    }
    /** @brief matrix multiplication */
    Mat3x3& operator*=(const Scalar other) {
        _data *= other;
        return *this;
    }

    /** @brief identity matix */
    static constexpr Mat3x3 identity() { return Mat3x3{}; }

    friend std::ostream& operator<<(std::ostream& os, const Mat3x3 mat);
    friend Mat3x3 operator+(const Mat3x3&, const Mat3x3&);
    friend Mat3x3 operator-(const Mat3x3&, const Mat3x3&);
    friend Mat3x3 operator*(const Mat3x3&, const Mat3x3&);
    friend Vec3 operator*(const Mat3x3&, const Vec3&);
    friend Vec3 operator*(const Vec3&, const Mat3x3&);
    friend Mat3x3 operator*(const Scalar, const Mat3x3&);
    friend Mat3x3 operator*(const Mat3x3&, const Scalar);

  private:
    using data_type = glm::mat<3, 3, Scalar>;
    constexpr Mat3x3(const data_type& data) : _data(data) {}
    constexpr Mat3x3(data_type&& data) : _data(std::move(data)) {}

    data_type _data;
};

/** @brief write matrix as space separated components */
inline std::ostream& operator<<(std::ostream& os, const Mat3x3 mat) {
    os << "Mat3x3(" << '\n'
       << mat._data[0][0] << ", " << mat._data[0][1] << ", " << mat._data[0][2]
       << "," << '\n'
       << mat._data[1][0] << ", " << mat._data[1][1] << ", " << mat._data[1][2]
       << "," << '\n'
       << mat._data[2][0] << ", " << mat._data[2][1] << ", " << mat._data[2][2]
       << '\n'
       << ")";
    return os;
}

/** @brief add elementwise */
inline Mat3x3 operator+(const Mat3x3& mat1, const Mat3x3& mat2) {
    return Mat3x3(mat1._data + mat2._data);
}

/** @brief subtract elementwise */
inline Mat3x3 operator-(const Mat3x3& mat1, const Mat3x3& mat2) {
    return Mat3x3(mat1._data - mat2._data);
}

/** @brief matrix multiplication */
inline Mat3x3 operator*(const Mat3x3& mat1, const Mat3x3& mat2) {
    return Mat3x3(mat1._data * mat2._data);
}
/** @brief matrix multiplication  */
inline Vec3 operator*(const Mat3x3& mat1, const Vec3& vec2) {
    return Vec3(mat1._data * vec2._data);
}
/** @brief matrix multiplication  */
inline Vec3 operator*(const Vec3& vec1, const Mat3x3& mat2) {
    return Vec3(vec1._data * mat2._data);
}

/** @brief scalar multiplication  */
inline Mat3x3 operator*(const Scalar f1, const Mat3x3& mat2) {
    return Mat3x3(f1 * mat2._data);
}
/** @brief scalar multiplication  */
inline Mat3x3 operator*(const Mat3x3& mat1, const Scalar f2) {
    return Mat3x3(f2 * mat1._data);
}

/**
 * @brief returns rotational matrix for given axis and amplitude
 * @note Rotations are performed around each axis separately in the order:
 *       `R_x`, `R_y`, `R_z` (roll, pitch, yaw, Trait-Bryan angles).
 * @see inverse_rotation_mat
 */
Mat3x3 rotation_mat(const Vec3& axis) {
    const Scalar cx = std::cos(axis.x());
    const Scalar sx = std::sin(axis.x());
    const Scalar cy = std::cos(axis.y());
    const Scalar sy = std::sin(axis.y());
    const Scalar cz = std::cos(axis.z());
    const Scalar sz = std::sin(axis.z());

    return Mat3x3{
        {cy * cz, sx * sy * cz - cx * sz, cx * sy * cz + sx * sz},
        {cy * sz, sx * sy * sz + cx * cz, cx * sy * sz - sx * cz},
        {-sy, sx * cy, cx * cy},
    };
}

/**
 * @brief returns inverse rotational matrix for given axis and amplitude
 * @note Rotations are performed around each inverted axis separately in the
 *       order: `R_(-z)`, `R_(-y)`, `R_(-x)` (yaw, pitch, roll, Trait-Bryan
 * angles).
 * @see rotation_mat
 */
Mat3x3 inverse_rotation_mat(const Vec3& axis) {
    const Scalar cx = std::cos(axis.x());
    const Scalar sx = std::sin(axis.x());
    const Scalar cy = std::cos(axis.y());
    const Scalar sy = std::sin(axis.y());
    const Scalar cz = std::cos(axis.z());
    const Scalar sz = std::sin(axis.z());

    return Mat3x3{
        {cy * cz, cy * sz, -sy},
        {sx * sy * cz - cx * sz, cx * cz + sx * sy * sz, sx * cy},
        {cx * sy * cz + sx * sz, -cz * sx + cx * sy * sz, cx * cy},
    };
}

/**
 * @brief returns scaling matrix for given scale coefficients
 * @see inverse_scaling_mat
 */
Mat3x3 scaling_mat(const Vec3& vec) {
    return Mat3x3{
        {vec.x(), 0.0, 0.0},
        {0.0, vec.y(), 0.0},
        {0.0, 0.0, vec.z()},
    };
}

/**
 * @brief returns inverse scaling matrix for given scale coefficients
 * @see scaling_mat
 */
Mat3x3 inverse_scaling_mat(const Vec3& vec) {
    return Mat3x3{
        {1.0 / vec.x(), 0.0, 0.0},
        {0.0, 1.0 / vec.y(), 0.0},
        {0.0, 0.0, 1.0 / vec.z()},
    };
}

} // namespace cpp_raytracing
#endif
