/**
 * @file
 * @brief non-Euclidean twisted orb geometry with Cartesian-like coordinates
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *twisted orb* coordinates:
 *       \code
 *        (u,v,z) = (
 *            sqrt(x^2 + y^2) *
 *                cos(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            sqrt(x^2 + y^2) *
 *                sin(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            z
 *        )
 *       \endcode
 *       For constants `psi` and `rho`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - psi * exp(-sqrt(x^2 + y^2 + z^2) / rho), z)
 *       \endcode
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_TWISTED_ORB_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_3D_TWISTED_ORB_GEOMETRY_HPP

#include "../../../world/geometries/runge_kutta_geometry.hpp"

namespace cpp_raytracing { namespace cartesian_3d {

/**
 * @brief non-Euclidean twisted orb geometry with Cartesian-like coordinates
 * @note vectors are with respect to the tangential space
 * @note The geometry is defined by the transformation form Carthesian space
 *       `(x,y,z)` to *twisted orb* coordinates:
 *       \code
 *        (u,v,z) = (
 *            sqrt(x^2 + y^2) *
 *                cos(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            sqrt(x^2 + y^2) *
 *                sin(psi * exp(-sqrt(x^2 + y^2 + z^2) / rho) - arctan(y, x)),
 *            z
 *        )
 *       \endcode
 *       For constants `psi` and `rho`.
 *       This is derived from the transformation in cylindric coordinates
 *      `(r, phi, z)`:
 *       \code
 *        (r,alpha,z) = (r, phi - psi * exp(-sqrt(x^2 + y^2 + z^2) / rho), z)
 *       \endcode
 */
class TwistedOrbGeometry : public RungeKuttaGeometry<3_D> {

  public:
    /**
     * @brief construct new geometry
     */
    TwistedOrbGeometry(const Scalar twist_angle, const Scalar twist_radius,
                       const Scalar ray_initial_step_size,
                       const Scalar ray_error_abs, const Scalar ray_error_rel,
                       const Scalar ray_max_length,
                       const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    TwistedOrbGeometry(const TwistedOrbGeometry&) = default;

    /** @brief move constructor */
    TwistedOrbGeometry(TwistedOrbGeometry&&) = default;

    /** @brief copy assignment */
    TwistedOrbGeometry& operator=(const TwistedOrbGeometry&) = delete;

    /** @brief move assignment */
    TwistedOrbGeometry& operator=(TwistedOrbGeometry&&) = delete;

    ~TwistedOrbGeometry() override = default;

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
     * @brief returns position in flat space coordinates
     * @note Public access for debugging only.
     * @note The methods to_onb_jacobian() and from_onb_jacobian() are with
     *       respect to this transformation to guarantee interoperability.
     * @see to_onb_jacobian(), from_onb_jacobian(), ray_from()
     */
    Vec3 to_cartesian_coords(const Vec3& position) const;

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

    /** @see RungeKuttaGeometry::treat_as_infinite_segment */
    bool treat_as_infinite_segment(const Vec3& position,
                                   const Vec3& velocity) const override;

  private:
    /** @brief maximal twist angle */
    Scalar _twist_angle;
    /** @brief controlls spatial extend of twisting  */
    Scalar _twist_radius;
};

TwistedOrbGeometry::TwistedOrbGeometry(const Scalar twist_angle,
                                       const Scalar twist_radius,
                                       const Scalar ray_initial_step_size,
                                       const Scalar ray_error_abs,
                                       const Scalar ray_error_rel,
                                       const Scalar ray_max_length,
                                       const Scalar ray_segment_length_factor)
    : RungeKuttaGeometry{ray_initial_step_size, ray_error_abs, ray_error_rel,
                         ray_max_length, ray_segment_length_factor},
      _twist_angle{twist_angle},
      _twist_radius{twist_radius} {}

std::unique_ptr<Ray3D>
TwistedOrbGeometry::ray_passing_through(const Vec3& start,
                                        const Vec3& target) const {
    using namespace tensor;

    // convert positions to flat space
    const Vec3 start_cart = to_cartesian_coords(start);
    const Vec3 target_cart = to_cartesian_coords(target);

    // in flat space direction = difference
    const Vec3 direction_cart = tensor::unit_vector(target_cart - start_cart);
    const Mat3x3 jacobian = from_onb_jacobian(start);

    // transform direction back to cuved space
    // note: direction is still normalized after transfomration
    const Vec3 direction = jacobian * direction_cart;

    return std::make_unique<RungeKuttaRay<3_D>>(*this, start, direction);
}

Mat3x3 TwistedOrbGeometry::to_onb_jacobian(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar r = std::sqrt(r2);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar t = std::exp(R / rho);
    const Scalar phi = 1.0 / t * psi + std::atan2(v, u);
    const Scalar cphi = std::cos(phi);
    const Scalar sphi = std::sin(phi);

    return {
        // clang-format off
        (u * cphi - r2 * (-v / r2 - u * psi / t / (R * rho)) * sphi) / r,
        (v * cphi - r2 * (u / r2 - v * psi / t / (R * rho)) * sphi) / r,
        (r * z * psi * sphi / t) / (R * rho),
       
        (r2 * (-v / r2 - u * psi / t / (R * rho)) * cphi + u * sphi) / r,
        (r2 * (u / r2 - v * psi / t / (R * rho)) * cphi + v * sphi) / r,
        -(r * z * psi * cphi / t) / (R * rho),

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 TwistedOrbGeometry::from_onb_jacobian(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar r = std::sqrt(r2);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar t = std::exp(R / rho);
    const Scalar phi = 1.0 / t * psi + std::atan2(v, u);
    const Scalar cphi = std::cos(phi);
    const Scalar sphi = std::sin(phi);

    return {
        // clang-format off
        ((u * R * rho - v * r2 * psi / t) * cphi + v * R * rho * sphi) /
            (r * R * rho),
        (-v * R * rho * cphi + (u * R * rho - v * r2 * psi / t) * sphi) /
            (r * R * rho),
        -v * z * psi / t / (R * rho),

        ((v * R * rho + u * r2 * psi / t) * cphi - u * R * rho * sphi) /
            (r * R * rho),
        (u * R * rho * cphi + (v * R * rho + u * r2 * psi / t) * sphi) /
            (r * R * rho),
        u * z * psi / t / (R * rho),

        0, 0, 1,
        // clang-format on
    };
}

Mat3x3 TwistedOrbGeometry::metric(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r2 = u * u + v * v;
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar s = std::exp(-2.0 * R / rho);
    const Scalar t = std::exp(R / rho);

    const Scalar f00 =
        1.0 + s * u * psi * (2.0 * t * v * rho / R + u * r2 * psi / R2) / rho2;
    const Scalar f01 = s * psi *
                       (-t * (u - v) * (u + v) * R * rho + u * v * r2 * psi) /
                       (rho2 * R2);
    const Scalar f02 =
        s * z * psi * (t * v * R * rho + u * r2 * psi) / (R2 * rho2);
    const Scalar f11 =
        1.0 + s * v * psi * (-2.0 * t * u * rho / R + v * r2 * psi / R2) / rho2;
    const Scalar f12 =
        s * z * psi * (-t * u * R * rho + v * r2 * psi) / (R2 * rho2);
    const Scalar f22 = 1.0 + s * r2 * z * z * psi * psi / (R2 * rho2);

    return {
        // clang-format off
        f00, f01, f02,
        f01, f11, f12,
        f02, f12, f22,
        // clang-format on
    };
}

Vec3 TwistedOrbGeometry::normalize(const Vec3& position,
                                   const Vec3& vec) const {
    using namespace tensor;

    return vec * (Scalar{1} / std::sqrt(dot(vec, metric(position) * vec)));
}

Vec3 TwistedOrbGeometry::to_cartesian_coords(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar r = std::sqrt(u * u + v * v);
    const Scalar R = std::sqrt(u * u + v * v + z * z);
    const Scalar phi = std::atan2(v, u) + psi * std::exp(-R / rho);
    return {r * std::cos(phi), r * std::sin(phi), z};
}

Mat3x3 TwistedOrbGeometry::inverse_metric(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar s = std::exp(-2.0 * R / rho);
    const Scalar t = std::exp(R / rho);

    const Scalar f00 =
        1.0 + (-2.0 / t * u * v * rho * psi / R + s * v * v * psi * psi) / rho2;
    const Scalar f01 =
        s * psi * (t * (u - v) * (u + v) * rho / R - u * v * psi) / rho2;
    const Scalar f02 = (-1.0 / t) * v * z * psi / (R * rho);
    const Scalar f11 =
        1.0 + (2.0 / t * u * v * rho * psi / R + s * u * u * psi * psi) / rho2;
    const Scalar f12 = (1.0 / t) * u * z * psi / (R * rho);
    const Scalar f22 = 1.0;

    return {
        // clang-format off
        f00, f01, f02,
        f01, f11, f12,
        f02, f12, f22,
        // clang-format on
    };
}

Ten3x3x3 TwistedOrbGeometry::christoffel_1(const Vec3& position) const {
    const Scalar psi = _twist_angle;
    const Scalar rho = _twist_radius;
    const Scalar rho2 = rho * rho;
    const Scalar rho3 = rho2 * rho;
    const Scalar u = position[0];
    const Scalar v = position[1];
    const Scalar z = position[2];
    const Scalar u2 = u * u;
    const Scalar u4 = u2 * u2;
    const Scalar u6 = u4 * u2;
    const Scalar v2 = v * v;
    const Scalar v4 = v2 * v2;
    const Scalar v6 = v4 * v2;
    const Scalar z2 = z * z;
    const Scalar z4 = z2 * z2;
    const Scalar R2 = u * u + v * v + z * z;
    const Scalar R = std::sqrt(R2);
    const Scalar R5 = R2 * R2 * R;
    const Scalar t = std::exp(R / rho);
    const Scalar s = std::exp(-2.0 * R / rho);

    // u

    const Scalar fuuu = s * psi *
                        (t * v * R2 * rho * (-u2 * R + (v2 + z2) * rho) -
                         u *
                             (u6 - v2 * (v2 + z2) * R * rho +
                              u2 * (v2 + z2) * (v2 - 2.0 * R * rho) +
                              u4 * (2.0 * v2 + z2 - R * rho)) *
                             psi) /
                        (R5 * rho3);

    const Scalar fuuv =
        -s * u * psi *
        (-t * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
         u * v * (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) *
             psi) /
        (R5 * rho3);

    const Scalar fuuz =
        -s * u * z * psi *
        (t * v * R2 * rho * (R + rho) + u * (u2 + v2) * (R2 + R * rho) * psi) /
        (R5 * rho3);

    const Scalar fuvv =
        s * psi *
        (t * v * R2 * rho * (-v2 * (R - 2.0 * rho) + 3.0 * (u2 + z2) * rho) -
         u *
             (v6 - 2.0 * v2 * z2 * R * rho + u4 * (v2 - R * rho) +
              u2 * (2.0 * v2 + z2) * (v2 - R * rho) + v4 * (z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fuvz = -s * z * psi *
                        (-t * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
                         u * v * (u2 + v2) * (R2 + R * rho) * psi) /
                        (R5 * rho3);

    const Scalar fuzz =
        s * psi *
        (t * v * R2 * rho * (-z2 * R + (u2 + v2) * rho) -
         u *
             (z4 * R * rho + u4 * (z2 - R * rho) + v4 * (z2 - R * rho) +
              v2 * (z4 + z2 * R * rho) +
              u2 * (z4 + z2 * R * rho + 2.0 * v2 * (z2 - R * rho))) *
             psi) /
        (R5 * rho3);

    // v

    const Scalar fvuu =
        s * psi *
        (t * u * R2 * rho * (u2 * (R - 2.0 * rho) - 3.0 * (v2 + z2) * rho) -
         v *
             (u6 - v2 * (v2 + z2) * R * rho +
              u2 * (v2 + z2) * (v2 - 2.0 * R * rho) +
              u4 * (2.0 * v2 + z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fvuv =
        -s * v * psi *
        (-t * R2 * rho * (u2 * R - (v2 + z2) * rho) +
         u * v * (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) *
             psi) /
        (R5 * rho3);

    const Scalar fvuz = -s * z * psi *
                        (-t * R2 * rho * (u2 * R - (v2 + z2) * rho) +
                         u * v * (u2 + v2) * (R2 + R * rho) * psi) /
                        (R5 * rho3);

    const Scalar fvvv =
        -s * psi *
        (t * u * R2 * rho * (-v2 * R + (u2 + z2) * rho) +
         v *
             (v6 - 2.0 * v2 * z2 * R * rho + u4 * (v2 - R * rho) +
              u2 * (2.0 * v2 + z2) * (v2 - R * rho) + v4 * (z2 - R * rho)) *
             psi) /
        (R5 * rho3);

    const Scalar fvvz =
        s * v * z * psi *
        (t * u * R2 * rho * (R + rho) - v * (u2 + v2) * (R2 + R * rho) * psi) /
        (R5 * rho3);

    const Scalar fvzz =
        -s * psi *
        (t * u * R2 * rho * (-z2 * R + (u2 + v2) * rho) +
         v *
             (z4 * R * rho + u4 * (z2 - R * rho) + v4 * (z2 - R * rho) +
              v2 * (z4 + z2 * R * rho) +
              u2 * (z4 + z2 * R * rho + 2.0 * v2 * (z2 - R * rho))) *
             psi) /
        (R5 * rho3);

    // z

    const Scalar fzuu =
        -s * z *
        (u6 - v2 * (v2 + z2) * R * rho + u2 * (v2 + z2) * (v2 - 3.0 * R * rho) +
         u4 * (2.0 * v2 + z2 - 2.0 * R * rho)) *
        psi * psi / (R5 * rho3);

    const Scalar fzuv = -s * u * v * z *
                        (u4 + v4 - 2.0 * z2 * R * rho + v2 * (z2 - R * rho) +
                         u2 * (2.0 * v2 + z2 - R * rho)) *
                        psi * psi / (R5 * rho3);

    const Scalar fzuz =
        -s * u * z2 *
        (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) * psi * psi /
        (R5 * rho3);

    const Scalar fzvv =
        s * z * R *
        (-v4 * (R - 2.0 * rho) + u4 * rho + 3.0 * v2 * z2 * rho +
         u2 * (-v2 * (R - 3.0 * rho) + z2 * rho)) *
        psi * psi / (R5 * rho3);

    const Scalar fzvz =
        -s * v * z2 *
        (u4 + v4 + v2 * z2 + u2 * (2.0 * v2 + z2) - z2 * R * rho) * psi * psi /
        (R5 * rho3);

    const Scalar fzzz = -s * (u2 + v2) * z *
                        (z4 + u2 * (z2 - R * rho) + v2 * (z2 - R * rho)) * psi *
                        psi / (R5 * rho3);

    return Ten3x3x3{
        // clang-format off
        // x
        fuuu, fuuv, fuuz,
        fuuv, fuvv, fuvz,
        fuuz, fuvz, fuzz,
        // y
        fvuu, fvuv, fvuz,
        fvuv, fvvv, fvvz,
        fvuz, fvvz, fvzz,
        // z
        fzuu, fzuv, fzuz,
        fzuv, fzvv, fzvz,
        fzuz, fzvz, fzzz,
        // clang-format on
    };
}

Ten3x3x3 TwistedOrbGeometry::christoffel_2(const Vec3& position) const {
    const Mat3x3 inv_metric = inverse_metric(position);
    const Ten3x3x3 chris_1 = christoffel_1(position);

    return gttl::contraction<1, 2>(inv_metric, chris_1);
}

bool TwistedOrbGeometry::treat_as_infinite_segment(const Vec3& position,
                                                   const Vec3& velocity) const {
    using namespace tensor;

    const Scalar R = length(position);
    const bool outwards = dot(position, velocity) > 0.0;
    return R / _twist_radius > 5.0 && outwards;
}

}} // namespace cpp_raytracing::cartesian_3d

#endif
