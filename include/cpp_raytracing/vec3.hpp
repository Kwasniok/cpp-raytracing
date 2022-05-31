/**
 * @file
 * @brief 3D vector representation
 */

#ifndef CPP_RAYTRACING_VEC3_HPP
#define CPP_RAYTRACING_VEC3_HPP

#include <array>
#include <cmath>
#include <iostream>

#include <glm/detail/qualifier.hpp>
#include <glm/geometric.hpp>

#include "random.hpp"
#include "scalar.hpp"

namespace cpp_raytracing {

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
    constexpr Vec3 operator-() const {
        return Vec3{-_data.x, -_data.y, -_data.z};
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

    friend inline constexpr Vec3 operator+(const Vec3& v1, const Vec3& v2);
    friend inline constexpr Vec3 operator-(const Vec3& v1, const Vec3& v2);
    friend inline constexpr Vec3 operator*(const Vec3& v, const Scalar f);
    friend inline constexpr Vec3 operator*(const Scalar f, const Vec3& v);
    friend inline constexpr Vec3 operator/(const Vec3& v, const Scalar f);
    friend inline constexpr Scalar dot(const Vec3& v1, const Vec3& v2);
    friend inline constexpr Vec3 cross(const Vec3& v1, const Vec3& v2);
    friend inline constexpr Vec3 unit_vector(const Vec3& v);

  private:
    using data_type = glm::vec<3, Scalar>;
    constexpr Vec3(const data_type data) : _data(data) {}

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

} // namespace cpp_raytracing
#endif
