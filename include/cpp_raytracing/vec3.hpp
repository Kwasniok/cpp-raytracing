/**
 * @file
 * @brief 3D vector representation
 */

#ifndef CPP_RAYTRACING_VEC3_HPP
#define CPP_RAYTRACING_VEC3_HPP

#include <cmath>
#include <iostream>

#include "random.hpp"
#include "scalar.hpp"

namespace ray {

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

    /** @brief initializecoefficients with random values */
    inline static Vec3 random(Scalar min, Scalar max) {
        return {random_scalar(min, max), random_scalar(min, max),
                random_scalar(min, max)};
    }

    /** @brief get x coefficient */
    constexpr Scalar x() const { return _data[0]; }
    /** @brief get y coefficient */
    constexpr Scalar y() const { return _data[1]; }
    /** @brief get z coefficient */
    constexpr Scalar z() const { return _data[2]; }

    /** @brief negate elementwise */
    constexpr Vec3 operator-() const {
        return Vec3{-_data[0], -_data[1], -_data[2]};
    }

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
        _data[0] += other._data[0];
        _data[1] += other._data[1];
        _data[2] += other._data[2];
        return *this;
    }
    /** @brief subtract elementwise */
    constexpr Vec3& operator-=(const Vec3& other) { return *this += (-other); }
    /** @brief multiply elementwise */
    constexpr Vec3& operator*=(const Scalar fac) {
        _data[0] *= fac;
        _data[1] *= fac;
        _data[2] *= fac;
        return *this;
    }
    /** @brief divide elementwise */
    constexpr Vec3& operator/=(const Scalar fac) { return *this *= (1 / fac); }

    /** @brief get length of the vector */
    constexpr Scalar length() const { return sqrt(length_squared()); }

    /**
     * @brief get length of the vector squared
     * @note Fater than std::pow(length(),2)`
     */
    constexpr Scalar length_squared() const {
        return _data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2];
    }

    /** @brief tests if vector is zero vector */
    constexpr bool near_zero(const Scalar epsilon) const {
        return std::abs(_data[0]) < epsilon && std::abs(_data[1]) < epsilon &&
               std::abs(_data[2]) < epsilon;
    }

  private:
    Scalar _data[3];
};

/** @brief write vector as space separated components */
inline std::ostream& operator<<(std::ostream& os, const Vec3 v) {
    os << v[0] << " " << v[1] << " " << v[2];
    return os;
}

/** @brief add elementwise */
inline constexpr Vec3 operator+(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
}

/** @brief subtract elementwise */
inline constexpr Vec3 operator-(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
}

/** @brief multiply elementwise */
inline constexpr Vec3 operator*(const Vec3& v, const Scalar f) {
    return Vec3(v[0] * f, v[1] * f, v[2] * f);
}

/** @brief multiply elementwise */
inline constexpr Vec3 operator*(const Scalar f, const Vec3& v) {
    return v * f;
}

/** @brief divide elementwise */
inline constexpr Vec3 operator/(const Vec3& v, const Scalar f) {
    const Scalar f_inv = 1 / f;
    return Vec3(v[0] * f_inv, v[1] * f_inv, v[2] * f_inv);
}

/** @brief scalar product (inner product) */
inline constexpr Scalar dot(const Vec3& v1, const Vec3& v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

/** @brief cross product (vector product) */
inline constexpr Vec3 cross(const Vec3& v1, const Vec3& v2) {
    return Vec3(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2],
                v1[0] * v2[1] - v1[1] * v2[0]);
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

} // namespace ray
#endif
