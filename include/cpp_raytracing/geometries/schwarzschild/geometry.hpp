/**
 * @file
 * @brief Cartesian (3+1) Schwarzschild geometry
 * @note The coordinates are (x, y, z, t).
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_GEOMETRY_HPP
#define CPP_RAYTRACING_GEOMETRIES_SCHWARZSCHILD_GEOMETRY_HPP

#include <exception>

#include "../../world/geometries/runge_kutta_geometry.hpp"

namespace cpp_raytracing { namespace schwarzschild {

/**
 * @brief Cartesian (x, y, z, t) Schwarzschild geometry
 */
class Geometry : public RungeKuttaGeometry<4_D> {

  public:
    /**
     * @brief construct new geometry
     */
    Geometry(const Scalar speed_of_light, const Scalar schwarzschild_radius,
             const Scalar ray_initial_step_size, const Scalar ray_error_abs,
             const Scalar ray_error_rel, const Scalar ray_max_length,
             const Scalar _ray_segment_length_factor);

    /** @brief copy constructor */
    Geometry(const Geometry&) = default;

    /** @brief move constructor */
    Geometry(Geometry&&) = default;

    /** @brief copy assignment */
    Geometry& operator=(const Geometry&) = delete;

    /** @brief move assignment */
    Geometry& operator=(Geometry&&) = delete;

    ~Geometry() override = default;

    /** @see Geometry::ray_passing_through */
    std::unique_ptr<Ray<4_D>>
    ray_passing_through(const Vec<4_D>& start,
                        const Vec<4_D>& target) const override;

    /** @see Geometry::to_onb_jacobian */
    Mat<3_D, 4_D> to_onb_jacobian(const Vec<4_D>& position) const override;
    /** @see Geometry::from_onb_jacobian */
    Mat<4_D, 3_D> from_onb_jacobian(const Vec<4_D>& position) const override;

    /** @see Geometry::metric */
    Mat<4_D> metric(const Vec<4_D>& position) const override;

  public:
    /**
     * @brief returns Christoffel symbols of second kind
     */
    TenR3<4_D> christoffel_2(const Vec<4_D>& position) const override;

  public:
    /** @brief speed of light */
    Scalar speed_of_light() const { return _speed_of_light; }
    /** @brief Schwarzschild radius */
    Scalar schwarzschild_radius() const { return _schwarzschild_radius; }

  private:
    /** @brief controlls speed of light  */
    Scalar _speed_of_light;
    /** @brief controlls spatial extend of black hole around origin  */
    Scalar _schwarzschild_radius;
};

Geometry::Geometry(const Scalar speed_of_light,
                   const Scalar schwarzschild_radius,
                   const Scalar ray_initial_step_size,
                   const Scalar ray_error_abs, const Scalar ray_error_rel,
                   const Scalar ray_max_length,
                   const Scalar ray_segment_length_factor)
    : RungeKuttaGeometry{ray_initial_step_size, ray_error_abs, ray_error_rel,
                         ray_max_length, ray_segment_length_factor},
      _speed_of_light{speed_of_light},
      _schwarzschild_radius{schwarzschild_radius} {}

std::unique_ptr<Ray<4_D>>
Geometry::ray_passing_through([[maybe_unused]] const Vec<4_D>& start,
                              [[maybe_unused]] const Vec<4_D>& target) const {
    using namespace tensor;

    throw std::logic_error("Geometry::ray_passing_through is not implemented.");
}

Mat<3_D, 4_D> Geometry::to_onb_jacobian(const Vec<4_D>& position) const {

    using std::sqrt, std::pow;
    using namespace tensor;

    const Scalar Rs = _schwarzschild_radius;
    const Scalar x = position[0];
    const Scalar y = position[1];
    const Scalar z = position[2];
    const Scalar r2 = pow(x, 2) + pow(y, 2) + pow(z, 2);
    const Scalar r = sqrt(r2);
    const Scalar rho = sqrt(pow(x, 2) + pow(y, 2));
    const Scalar alpha = pow(1 - Rs / r, -0.5);

    // Cartesian to spherical contravariant Jacobian and normalization
    return {
        Vec<4_D>{x / (r * alpha), (x * z) / (rho * alpha), -y / alpha, 0},
        Vec<4_D>{y / r2, (y * z) / (r * rho), x / r, 0},
        Vec<4_D>{z / (r * rho), -1, 0, 0},
    };
}

Mat<4_D, 3_D> Geometry::from_onb_jacobian(const Vec<4_D>& position) const {

    using std::sqrt, std::pow;
    using namespace tensor;

    const Scalar Rs = _schwarzschild_radius;
    const Scalar x = position[0];
    const Scalar y = position[1];
    const Scalar z = position[2];
    const Scalar r2 = pow(x, 2) + pow(y, 2) + pow(z, 2);
    const Scalar r = sqrt(r2);
    const Scalar rho2 = pow(x, 2) + pow(y, 2);
    const Scalar rho = sqrt(rho2);
    const Scalar alpha = pow(1 - Rs / r, -0.5);

    // denormalize and spherical to Cartesian contravariant Jacobian
    return {
        Vec<3_D>{(alpha * x) / r, y, (rho * z) / r},
        Vec<3_D>{(alpha * x * z / (rho * r2)), (y * z) / (rho * r), -rho2 / r2},
        Vec<3_D>{(-alpha * y) / rho2, (r * x) / rho2, 0},
        Vec<3_D>{0, 0, 0},
    };
}

Mat<4_D> Geometry::metric(const Vec<4_D>& position) const {

    using std::sqrt, std::pow, std::atan2;
    using namespace tensor;

    const Scalar c = _speed_of_light;
    const Scalar Rs = _schwarzschild_radius;
    const Scalar x = position[0];
    const Scalar y = position[1];
    const Scalar z = position[2];
    const Scalar r2 = pow(x, 2) + pow(y, 2) + pow(z, 2);
    const Scalar r = sqrt(r2);

    return {
        // clang-format off
        (pow(y, 2) + pow(z, 2)) / r2 + pow(x, 2) / (r2 - r * Rs),
        (x * y * Rs) / (r2 * (r - Rs)),
        (x * z * Rs) / (r2 * (r - Rs)),
        0,

        (x * y * Rs) / (r2 * (r - Rs)),
        (pow(x, 2) + pow(z, 2)) / r2 + pow(y, 2) / (r2 - r * Rs),
        (y * z * Rs) / (r2 * (r - Rs)),
        0,

        (x * z * Rs) / (r2 * (r - Rs)),
        (y * z * Rs) / (r2 * (r - Rs)),
        (pow(x, 2) + pow(y, 2)) / r2 + pow(z, 2) / (r2 - r * Rs),
        0,

        0,
        0,
        0,
        pow(c, 2) * (-1 + Rs / r),
        // clang-format on
    };
}

TenR3<4_D> Geometry::christoffel_2(const Vec<4_D>& position) const {

    using std::sqrt, std::pow, std::atan2;
    using namespace tensor;

    const Scalar c = _speed_of_light;
    const Scalar c2 = pow(c, 2);
    const Scalar Rs = _schwarzschild_radius;
    const Scalar Rs2 = pow(Rs, 2);
    const Scalar Rs3 = pow(Rs, 3);
    const Scalar Rs4 = pow(Rs, 4);
    const Scalar Rs5 = pow(Rs, 5);
    const Scalar Rs6 = pow(Rs, 6);
    const Scalar Rs7 = pow(Rs, 7);
    const Scalar x = position[0];
    const Scalar y = position[1];
    const Scalar z = position[2];
    const Scalar x2 = pow(x, 2);
    const Scalar y2 = pow(y, 2);
    const Scalar z2 = pow(z, 2);
    const Scalar x4 = pow(x, 4);
    const Scalar y4 = pow(y, 4);
    const Scalar z4 = pow(z, 4);
    const Scalar x6 = pow(x, 6);
    const Scalar y6 = pow(y, 6);
    const Scalar z6 = pow(z, 6);
    const Scalar x8 = pow(x, 8);
    const Scalar y8 = pow(y, 8);
    const Scalar z8 = pow(z, 8);
    const Scalar x10 = pow(x, 10);
    const Scalar y10 = pow(y, 10);
    const Scalar r = sqrt(x2 + y2 + z2);
    const Scalar r2 = pow(r, 2);
    const Scalar r5 = pow(r, 5);
    const Scalar r7 = pow(r, 7);

    return {
        // clang-format off
        -((x * (r - Rs) * Rs * (x4 - x2 * (y2 + z2) - 2 *(y2 + z2) * (y2 + z2 - r * Rs)) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r2 - r * Rs) * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x2 * y * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x2 * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x2 * y * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        (x * (r - Rs) * Rs * (2 * x4 - y4 + y2 * z2 + 2 * z4 - 2 * z2 * r * Rs + x2 * (y2 + 4 * z2 - 2 * r * Rs)) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))), 
        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x2 * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        (x * (r - Rs) * Rs * (2 * x8 + 2 * y8 - z8 + 5 * z6 * r * Rs - 10 * z6 * Rs2 + 10 * z4 * r * Rs3 - 5 * z4 * Rs4 + z2 * r * Rs5 - y2 * (z6 + 2 * z4 * (r - 5 * Rs) * Rs + 5 * z2 * (6 * r - 5 * Rs) * Rs3 + 2 *(6 * r - Rs) * Rs5) + y6 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + x6 * (8 * y2 + 5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + y4 * (3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs)) + x4 * (12 * y4 + 3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs) + 3 * y2 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs))) + x2 * (8 * y6 - z6 - 2 * z4 * (r - 5 * Rs) * Rs - 5 * z2 * (6 * r - 5 * Rs) * Rs3 + 2 * Rs5 * (-6 * r + Rs) + 3 * y4 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + 2 * y2 * (3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs))))) / (2 * r5 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))), 
        0, 

        0, 
        0, 
        0, 
        (c2 * x * (r - Rs) * Rs * (r2 - r * Rs) * (x4 + y4 + z4 - 4 * z2 * r * Rs + 6 * z2 * Rs2 - 4 * r * Rs3 + Rs4 + 2 * y2 * (z2 - 2 * r * Rs + 3 * Rs2) + 2 * x2 * (y2 + z2 - 2 * r * Rs + 3 * Rs2))) / (2 * r5 * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))),

        -((y * (r - Rs) * Rs * (x4 - x2 * (y2 + z2) - 2 *(y2 + z2) * (y2 + z2 - r * Rs)) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x * y2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x * y2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((y * (r - Rs) * Rs * (-2 * x4 + y4 - y2 * z2 - 2 * z4 + 2 * z2 * r * Rs - x2 * (y2 + 4 * z2 - 2 * r * Rs)) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r2 - r * Rs) * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((y2 * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((y2 * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        (y * (r - Rs) * Rs * (2 * x8 + 2 * y8 - z8 + 5 * z6 * r * Rs - 10 * z6 * Rs2 + 10 * z4 * r * Rs3 - 5 * z4 * Rs4 + z2 * r * Rs5 - y2 * (z6 + 2 * z4 * (r - 5 * Rs) * Rs + 5 * z2 * (6 * r - 5 * Rs) * Rs3 + 2 *(6 * r - Rs) * Rs5) + y6 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + x6 * (8 * y2 + 5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + y4 * (3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs)) + x4 * (12 * y4 + 3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs) + 3 * y2 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs))) + x2 * (8 * y6 - z6 - 2 * z4 * (r - 5 * Rs) * Rs - 5 * z2 * (6 * r - 5 * Rs) * Rs3 + 2 * Rs5 * (-6 * r + Rs) + 3 * y4 * (5 * z2 + 6 * Rs * (-2 * r + 5 * Rs)) + 2 * y2 * (3 * z4 + 10 * Rs3 * (-4 * r + 3 * Rs) + z2 * Rs * (-19 * r + 50 * Rs))))) / (2 * r5 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))), 
        0, 

        0, 
        0, 
        0, 
        (c2 * y * (r - Rs) * Rs * (r2 - r * Rs) * (x4 + y4 + z4 - 4 * z2 * r * Rs + 6 * z2 * Rs2 - 4 * r * Rs3 + Rs4 + 2 * y2 * (z2 - 2 * r * Rs + 3 * Rs2) + 2 * x2 * (y2 + z2 - 2 * r * Rs + 3 * Rs2))) / (2 * r5 * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))),

        -((z * (r - Rs) * Rs * (x4 - x2 * (y2 + z2) - 2 *(y2 + z2) * (y2 + z2 - r * Rs)) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((x * z2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x * y * z * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r - Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        (z * (r - Rs) * Rs * (2 * x4 - y4 + y2 * z2 + 2 * z4 - 2 * z2 * r * Rs + x2 * (y2 + 4 * z2 - 2 * r * Rs)) * (x6 + y6 + z6 - 5 * z4 * r * Rs + 10 * z4 * Rs2 - 10 * z2 * r * Rs3 + 5 * z2 * Rs4 - r * Rs5 + y2 * (3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs)) + y4 * (3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x4 * (3 * y2 + 3 * z2 + 5 * Rs * (-r + 2 * Rs)) + x2 * (3 * y4 + 3 * z4 - 10 * z2 * (r - 2 * Rs) * Rs + 5 * Rs3 * (-2 * r + Rs) + 2 * y2 * (3 * z2 + 5 * Rs * (-r + 2 * Rs))))) / (2 * r7 * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))), 
        -((y * z2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        0, 

        -((x * z2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        -((y * z2 * Rs * (3 * x2 + 3 * y2 + 3 * z2 - 2 * r * Rs) * (x6 + y6 + z6 - 6 * z4 * r * Rs + 15 * z4 * Rs2 - 20 * z2 * r * Rs3 + 15 * z2 * Rs4 - 6 * r * Rs5 + Rs6 + y2 * (3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs)) + 3 * y4 * (z2 + Rs * (-2 * r + 5 * Rs)) + 3 * x4 * (y2 + z2 + Rs * (-2 * r + 5 * Rs)) + x2 * (3 * y4 + 3 * z4 - 6 * z2 * (2 * r - 5 * Rs) * Rs + 5 * Rs3 * (-4 * r + 3 * Rs) + 6 * y2 * (z2 + Rs * (-2 * r + 5 * Rs))))) / (2 * r5 * (r - Rs) * (r2 - r * Rs) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2)))), 
        (z * (r - Rs) * Rs * (2 * x10 + 2 * y10 + 7 * y8 * (z2 - 2 *(r - 3 * Rs) * Rs) - y2 * (2 * z8 + 10 * z4 * (3 * r - 4 * Rs) * Rs3 + z2 * (36 * r - 13 * Rs) * Rs5 + 2 * r * Rs7 + z6 * Rs * (-4 * r + 3 * Rs)) - z4 * (z6 - 5 * z2 * (4 * r - 3 * Rs) * Rs3 + Rs5 * (-6 * r + Rs) + 3 * z4 * Rs * (-2 * r + 5 * Rs)) + y4 * (2 * z6 - 5 * z2 * (24 * r - 25 * Rs) * Rs3 + 14 * Rs5 * (-3 * r + Rs) + 3 * z4 * Rs * (-8 * r + 27 * Rs)) + y6 * (8 * z4 + 70 * Rs3 * (-r + Rs) + 3 * z2 * Rs * (-12 * r + 37 * Rs)) + x8 * (10 * y2 + 7 *(z2 - 2 *(r - 3 * Rs) * Rs)) + x6 * (20 * y4 + 8 * z4 + 70 * Rs3 * (-r + Rs) + 3 * z2 * Rs * (-12 * r + 37 * Rs) + 28 * y2 * (z2 - 2 *(r - 3 * Rs) * Rs)) + x4 * (20 * y6 + 2 * z6 - 5 * z2 * (24 * r - 25 * Rs) * Rs3 + 14 * Rs5 * (-3 * r + Rs) + 3 * z4 * Rs * (-8 * r + 27 * Rs) + 42 * y4 * (z2 - 2 *(r - 3 * Rs) * Rs) + 3 * y2 * (8 * z4 + 70 * Rs3 * (-r + Rs) + 3 * z2 * Rs * (-12 * r + 37 * Rs))) + x2 * (10 * y8 - 2 * z8 + z6 * (4 * r - 3 * Rs) * Rs - 2 * r * Rs7 + 10 * z4 * Rs3 * (-3 * r + 4 * Rs) + z2 * Rs5 * (-36 * r + 13 * Rs) + 28 * y6 * (z2 - 2 *(r - 3 * Rs) * Rs) + 2 * y2 * (2 * z6 - 5 * z2 * (24 * r - 25 * Rs) * Rs3 + 14 * Rs5 * (-3 * r + Rs) + 3 * z4 * Rs * (-8 * r + 27 * Rs)) + 3 * y4 * (8 * z4 + 70 * Rs3 * (-r + Rs) + 3 * z2 * Rs * (-12 * r + 37 * Rs))))) / (2 * r5 * (r2 - r * Rs) * (r2 - 2 * r * Rs + Rs2) * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))), 
        0, 

        0, 
        0, 
        0, 
        (c2 * z * (r - Rs) * Rs * (r2 - r * Rs) * (x4 + y4 + z4 - 4 * z2 * r * Rs + 6 * z2 * Rs2 - 4 * r * Rs3 + Rs4 + 2 * y2 * (z2 - 2 * r * Rs + 3 * Rs2) + 2 * x2 * (y2 + z2 - 2 * r * Rs + 3 * Rs2))) / (2 * r5 * (z4 * r + x4 * (r - 5 * Rs) + y4 * (r - 5 * Rs) - 5 * z4 * Rs + 10 * z2 * r * Rs2 - 10 * z2 * Rs3 + 5 * r * Rs4 - Rs5 + 2 * y2 * (z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2) + 2 * x2 * (y2 * (r - 5 * Rs) + z2 * (r - 5 * Rs) + 5 *(r - Rs) * Rs2))),

        0, 
        0, 
        0, 
        (x * Rs) / (2 * r2 * (r - Rs)), 

        0, 
        0, 
        0, 
        (y * Rs) / (2 * r2 * (r - Rs)), 

        0, 
        0, 
        0, 
        (z * Rs) / (2 * r2 * (r - Rs)), 

        (x * Rs) / (2 * r2 * (r - Rs)), 
        (y * Rs) / (2 * r2 * (r - Rs)), 
        (z * Rs) / (2 * r2 * (r - Rs)), 
        0,
        // clang-format on
    };
}

}} // namespace cpp_raytracing::schwarzschild

#endif
