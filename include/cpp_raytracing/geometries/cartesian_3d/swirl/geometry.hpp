/**
 * @file
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_SWIRL_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_SWIRL_GEOMETRY_HPP

#include "../../../world/geometries/runge_kutta_geometry.hpp"

namespace cpp_raytracing { namespace cartesian_3d {

/**
 * @brief non-Euclidean swirl geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *swirl* coordinates:
 *       \code
 *        (u,v,z) = (
 *            r * cos(phi - a * r * z),
 *            r * sin(phi - a * r * z),
 *            z
 *        )
 *       where r = sqrt(x * x + y * y), phi = arctan(y / x)
 *       \endcode
 *       For constant `a`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - a * r * z, z)
 *       \endcode
 */
class SwirlGeometry : public RungeKuttaGeometry<3_D> {

  public:
    /**
     * @brief construct new geometry
     * @note `swirl_strength = 0.0` is equivalent to Euclidean space.
     */
    SwirlGeometry(const Scalar swirl_strength,
                  const Scalar ray_initial_step_size,
                  const Scalar ray_error_abs, const Scalar ray_error_rel,
                  const Scalar ray_max_length,
                  const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    SwirlGeometry(const SwirlGeometry&) = default;

    /** @brief move constructor */
    SwirlGeometry(SwirlGeometry&&) = default;

    /** @brief copy assignment */
    SwirlGeometry& operator=(const SwirlGeometry&) = delete;

    /** @brief move assignment */
    SwirlGeometry& operator=(SwirlGeometry&&) = delete;

    ~SwirlGeometry() override = default;

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray3D>
    ray_passing_through(const Vec3& start, const Vec3& target) const override;

    /** @see Geometry::to_onb_jacobian */
    Mat3x3 to_onb_jacobian(const Vec3& position) const override;
    /** @see Geometry::from_onb_jacobian */
    Mat3x3 from_onb_jacobian(const Vec3& position) const override;

    /** @see Geometry::metric */
    Mat3x3 metric(const Vec3& position) const override;

  public:
    /** @brief returns normalized vector */
    Vec3 normalize(const Vec3& position, const Vec3& vec) const;

    /**
     * @brief returns inverse metric
     * @note Public access for debugging only.
     * @see metric()
     */
    Mat3x3 inverse_metric(const Vec3& position) const;
    /**
     * @brief returns Chirstoffel symbols of first kind
     * @note Public access for debugging only.
     */
    Ten3x3x3 christoffel_1(const Vec3& position) const;

    /**
     * @brief returns Chirstoffel symbols of second kind
     */
    Ten3x3x3 christoffel_2(const Vec3& position) const override;

  private:
    /** @brief strength of the swirl effect */
    Scalar _swirl_strength;
};

SwirlGeometry::SwirlGeometry(const Scalar swirl_strength,
                             const Scalar ray_initial_step_size,
                             const Scalar ray_error_abs,
                             const Scalar ray_error_rel,
                             const Scalar ray_max_length,
                             const Scalar ray_segment_length_factor)
    : RungeKuttaGeometry{ray_initial_step_size, ray_error_abs, ray_error_rel,
                         ray_max_length, ray_segment_length_factor},
      _swirl_strength{swirl_strength} {}

std::unique_ptr<Ray3D>
SwirlGeometry::ray_passing_through(const Vec3& start,
                                   const Vec3& target) const {

    const Scalar a = _swirl_strength;

    // start
    const Scalar u0 = start[0];
    const Scalar v0 = start[1];
    const Scalar z0 = start[2];
    const Scalar r0 = std::sqrt(u0 * u0 + v0 * v0);
    const Scalar arz0 = a * r0 * z0;
    const Scalar phi0 = std::atan2(v0, u0) + arz0;

    // target
    const Scalar u1 = target[0];
    const Scalar v1 = target[1];
    const Scalar z1 = target[2];
    const Scalar r1 = std::sqrt(u1 * u1 + v1 * v1);
    const Scalar arz1 = a * r1 * z1;
    const Scalar phi1 = std::atan2(v1, u1) + arz1;

    Vec3 direction = {
        -r0 * std::cos(arz0 - phi0) + r1 * std::cos(arz0 - phi1) -
            a * r0 *
                (-2 * r0 * z0 + r0 * z1 + r1 * z0 * std::cos(phi1 - phi0)) *
                std::sin(arz0 - phi0),
        a * r0 * r0 * (2 * z0 - z1) * std::cos(arz0 - phi0) -
            0.5 * arz0 * r1 *
                (std::cos(arz0 - phi1) + std::cos(arz0 - 2 * phi0 + phi1)) +
            r0 * std::sin(arz0 - phi0) - r1 * std::sin(arz0 - phi1),
        z1 - z0,
    };

    return std::make_unique<RungeKuttaRay<3_D>>(*this, start,
                                                normalize(start, direction));
}

Mat3x3 SwirlGeometry::to_onb_jacobian(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    // note: convention is atan2(y, x)
    const Scalar phi = std::atan2(v, u) + a * r * z;
    const Scalar sphi = std::sin(phi);
    const Scalar cphi = std::cos(phi);

    return {
        // clang-format off
        ((u + a * v * z * r) * cphi + v * sphi) / r,
        ((u + a * v * z * r) * sphi - v * cphi) / r,
        a * v * r,

        ((v - a * u * z * r) * cphi - u * sphi) / r,
        ((v - a * u * z * r) * sphi + u * cphi) / r,
        -a * u * r,

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 SwirlGeometry::from_onb_jacobian(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    // note: convention is atan2(y, x)
    const Scalar phi = std::atan2(v, u) + a * r * z;
    const Scalar sphi = std::sin(phi);
    const Scalar cphi = std::cos(phi);

    return {
        // clang-format off
        ((v - a * u * z * r) * sphi + u * cphi) / r,
        (-(u + a * v * z * r) * sphi + v * cphi) / r,
        -a * r * r * sphi,

        ((-v + a * u * z * r) * cphi + u * sphi) / r,
        ((u + a * v * z * r) * cphi + v * sphi) / r,
        a * r * r * cphi,

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 SwirlGeometry::metric(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);

    return {
        // clang-format off
            1 + a * u * z * (a * u * z - 2 * v / r),
            a * z * (u * u - v * v + a * u * v * z * r) / r,
            a * r * (a * u * z * r - v),
   
            (a * z * (u * u - v * v + a * u * v * z * r)) / r,
            1 + a * v * z * (2 * u / r + a * v * z),
            a * (u * r + a * v * z * r * r),

            a * r * (a * u * z * r - v),
            a * r * (u + a * v * z * r),
            1 + a * a * r * r * r * r,
        // clang-format on
    };
}

Vec3 SwirlGeometry::normalize(const Vec3& position, const Vec3& vec) const {
    using namespace tensor;

    return vec * (Scalar{1} / std::sqrt(dot(vec, metric(position) * vec)));
}

Mat3x3 SwirlGeometry::inverse_metric(const Vec3& position) const {
    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    const Scalar s = u * u - v * v;
    const Scalar u2v2z2 = u * u + v * v + z * z;

    return {
        // clang-format off
        1 + a * v * ((2 * u * z) / r + a * v * (r * r + z * z)),
        a * ((-s * z) / r - a * u * v * u2v2z2),
        a * v * r,

        a * ((-s * z) / r - a * u * v * u2v2z2),
        1 + a * u * ((-2 * v * z) / r + a * u * u2v2z2),
        -a * u * r,
      
        a * v * r,
        -a * u * r,
        1,
        // clang-format on
    };
}

Ten3x3x3 SwirlGeometry::christoffel_1(const Vec3& position) const {

    const Scalar a = _swirl_strength;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);

    const Scalar arz = a * r * z;
    const Scalar a2r2 = a * a * r * r;
    const Scalar a2r3 = a * a * r * r * r;

    const Scalar alpha = std::atan2(v, u);

    // trigonometric
    const Scalar cos_alpha = std::cos(alpha);
    const Scalar sin_alpha = std::sin(alpha);
    const Scalar cos_2alpha = std::cos(2 * alpha);
    const Scalar sin_2alpha = std::sin(2 * alpha);
    const Scalar cos_3alpha = std::cos(3 * alpha);
    const Scalar sin_3alpha = std::sin(3 * alpha);

    // trigonometric powers
    const Scalar cos3_alpha = std::pow(cos_alpha, 3);
    const Scalar sin3_alpha = std::pow(sin_alpha, 3);

    return {
        // clang-format off
        // x
        a * z * (arz * cos_alpha - sin3_alpha),
        -a * z * cos3_alpha,
        a * r * cos_alpha * (arz * cos_alpha - sin_alpha),

        -a * z * cos3_alpha,
        -0.25 * a * z * (-4 * arz * cos_alpha + 9 * sin_alpha + sin_3alpha),
        0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),

        a * r * cos_alpha * (arz * cos_alpha - sin_alpha),
        0.5 * a * r * (-3 + cos_2alpha + arz * sin_2alpha),
        -a2r3 * cos_alpha,

        // y
        0.25 * a * z * (9 * cos_alpha - cos_3alpha + 4 * arz * sin_alpha),
        a * z * sin3_alpha,
        0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),

        a * z * sin3_alpha,
        a * z * (cos3_alpha + arz * sin_alpha),
        a * r * sin_alpha * (cos_alpha + arz * sin_alpha),

        0.5 * a * r * (3 + cos_2alpha + arz * sin_2alpha),
        a * r * sin_alpha * (cos_alpha + arz * sin_alpha),
        -a2r3 * sin_alpha,

        // z
        0.5 * a2r2 * z * (3 + cos_2alpha),
        a2r2 * z * cos_alpha * sin_alpha,
        2 * a2r3 * cos_alpha,

        a2r2 * z * cos_alpha * sin_alpha,
        -(0.5) * a2r2 * z * (-3 + cos_2alpha),
        2 * a2r3 * sin_alpha,

        2 * a2r3 * cos_alpha,
        2 * a2r3 * sin_alpha,
        0,
        // clang-format on
    };
}

Ten3x3x3 SwirlGeometry::christoffel_2(const Vec3& position) const {
    const Mat3x3 inv_metric = inverse_metric(position);
    const Ten3x3x3 chris_1 = christoffel_1(position);

    return gttl::contraction<1, 2>(inv_metric, chris_1);
}

}} // namespace cpp_raytracing::cartesian_3d

#endif
