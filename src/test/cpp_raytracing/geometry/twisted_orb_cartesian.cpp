#include <vector>

#include <cpp_raytracing/geometry/twisted_orb_cartesian.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-12;
constexpr Scalar ray_step_size = 0.1;

void test_ray_passing_through() {

    using std::exp, std::cos, std::sin, std::atan2, std::sqrt;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    constexpr Vec3 start = {2, -3, 5};
    constexpr Vec3 target = {-7, 11, -13};

    constexpr Vec3 direction_non_normalized = {
        -2.0 -
            sqrt(170.0) *
                cos(pi / 8 *
                        (exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) +
                    atan2(11, 7)) -
            exp(-sqrt(38.0) / 17.0) * pi / (272.0 * sqrt(38.0)) *
                (618.0 + 13.0 * sqrt(170.0) *
                             (cos(((exp(-sqrt(38.0) / 17.0) -
                                    exp(-sqrt(339.0) / 17.0)) *
                                       pi +
                                   4 * atan2(13483.0, 4956.0)) /
                                  8.0) +
                              sin(((exp(-sqrt(38.0) / 17.0) -
                                    exp(-sqrt(339.0) / 17.0)) *
                                       pi / 8.0 +
                                   atan2(11.0, 7.0))))),

        1.0 / (68.0 * sqrt(38.0)) *
            (exp(-sqrt(38.0) / 17.0) * pi *
                 (-103.0 -
                  47.0 *
                      cos((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) *
                          pi / 8.0) +
                  sin((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) *
                      pi / 8.0)) +
             68.0 * (3.0 * sqrt(38.0) + 2.0 * sqrt(1615.0) *
                                            sin((exp(-sqrt(38.0) / 17.0) -
                                                 exp(-sqrt(339.0) / 17.0)) *
                                                    pi / 8.0 +
                                                atan2(11.0, 7.0)))),

        -18.0,
    };
    // numericaly: {-8.56156, 14.0917, -18}

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    const Vec3 direction = geometry.normalize(start, direction_non_normalized);
    // numericaly: {-0.349584, 0.575389, -0.734972}

    auto ray = geometry.ray_passing_through(start, target);
    const Vec6 initial_phase =
        dynamic_cast<TwistedOrbCartesianRay*>(ray.get())->phase();
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(initial_phase.first_half(), start,
                                      epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(initial_phase.second_half(), direction,
                                      epsilon);
}

void test_to_onb_jacobian() {

    using std::exp, std::cos, std::sin, std::sqrt, std::atan2;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    const std::vector<std::pair<const Vec3, const Mat3x3>>
        points_and_jacobians = {
            {
                Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                Mat3x3{
                    Vec3{
                        cos(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (5168.0 * exp(19.0 / 510.0)),
                        -sin(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (7752.0 * exp(19.0 / 510.0)),
                        sqrt(13.0) * pi *
                            sin(pi / (8.0 * exp(19.0 / 510.0)) + atan2(2, 3)) /
                            (2584.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{
                        sin(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (5168.0 * exp(19.0 / 510.0)),
                        cos(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (7752.0 * exp(19.0 / 510.0)),
                        -sqrt(13.0) * pi *
                            cos(pi / (8.0 * exp(19.0 / 510.0)) + atan2(2, 3)) /
                            (2584.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{0, 0, 1},
                },
                /* numerically:
                {
                    {0.937967, -0.363585, 0.00347496},
                    {0.363376, 0.925279, -0.00240015},
                    0., 0., 1.}
                }
                 */
            },
        };

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.to_onb_jacobian(point),
                                          jacobian, epsilon);
    }
}

void test_from_onb_jacobian() {

    using std::exp, std::cos, std::sin;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    const std::vector<std::pair<const Vec3, const Mat3x3>>
        points_and_jacobians = {
            {
                Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                Mat3x3{
                    Vec3{
                        cos(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (7752.0 * exp(19.0 / 510.0)),
                        sin(pi / (8.0 * exp(19.0 / 510.0))) -
                            5.0 * pi *
                                (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (7752.0 * exp(19.0 / 510.0)),
                        -pi / (1292.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{
                        (15.0 * pi * cos(pi / (8.0 * exp(19.0 / 510.0))) -
                         2.0 * (2584.0 * exp(19.0 / 510.0) + 5 * pi) *
                             sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                            (5168.0 * exp(19.0 / 510.0)),
                        cos(pi / (8.0 * exp(19.0 / 510.0))) +
                            5.0 * pi *
                                (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) +
                                 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) /
                                (5168.0 * exp(19.0 / 510.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{0, 0, 1},
                },
                /* numerically:
                {
                    {0.925279, 0.363585, -0.00234265},
                    {-0.363376, 0.937967, 0.00351398},
                    {0, 0, 1}
                }
                 */
            },
        };

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.from_onb_jacobian(point),
                                          jacobian, epsilon);
    }
}

void test_metric() {

    using std::exp;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    const std::vector<std::pair<const Vec3, const Mat3x3>> points_and_metrics =
        {
            {
                Vec3{2.0, 0.0, 0.0},
                Mat3x3{
                    Vec3{
                        1.0 + pi * pi / (4624.0 * exp(4.0 / 17.0)),
                        -pi / (68.0 * exp(2.0 / 17.0)),
                        0.0,
                    },
                    Vec3{
                        -pi / (68.0 * exp(2.0 / 17.0)),
                        1.0,
                        0.0,
                    },
                    Vec3{0.0, 0.0, 1.0},
                },
            },
            {
                Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                Mat3x3{
                    Vec3{
                        1.0 + 5.0 * pi *
                                  (20672.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                                  (26708224.0 * exp(19.0 / 255.0)),
                        25.0 * pi * (-2584.0 * exp(19.0 / 510) + 13.0 * pi) /
                            (40062336.0 * exp(19.0 / 255.0)),
                        pi * (10336.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                            (13354112.0 * exp(19.0 / 255.0)),
                    },
                    Vec3{
                        25.0 * pi * (-2584 * exp(19.0 / 510) + 13.0 * pi) /
                            (40062336.0 * exp(19.0 / 255.0)),
                        1.0 + 5.0 * pi *
                                  (-46512.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                                  (60093504.0 * exp(19.0 / 255.0)),
                        pi * (-23256.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                            (20031168.0 * exp(19.0 / 255.0)),
                    },
                    Vec3{
                        pi * (10336.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                            (13354112.0 * exp(19.0 / 255.0)),
                        pi * (-23256.0 * exp(19.0 / 510.0) + 65.0 * pi) /
                            (20031168.0 * exp(19.0 / 255.0)),
                        1.0 + 13 * pi * pi / (6677056.0 * exp(19.0 / 255.0)),
                    },
                },
                /* numerically:
                    {
                        {1.01182, -0.00480621, 0.00238724},
                        {-0.00480621, 0.988336, -0.00348425},
                        {0.00238724, -0.00348425, 1.00002}
                    }
                */
            },
        };

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    for (const auto& [point, metric] : points_and_metrics) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.metric(point), metric,
                                          epsilon);
    }
}

void test_to_cartesian_coords() {

    using std::exp, std::cos, std::sin, std::sqrt, std::atan;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    constexpr Vec3 position = {
        +sqrt(5.0) * cos(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
        -sqrt(5.0) * sin(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
        3.0,
    };
    // numericaly: {1.57062, 1.59159, 3}
    constexpr Vec3 position_cart = {1.0, 2.0, 3.0};

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.to_cartesian_coords(position),
                                      position_cart, epsilon);
}

void test_inverse_metric() {

    using std::exp;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    const std::vector<std::pair<const Vec3, const Mat3x3>>
        points_and_inv_metrics = {
            {
                Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                Mat3x3{
                    Vec3{
                        1.0 - 5.0 * pi / (1292.0 * exp(19.0 / 510.0)) +
                            pi * pi / (166464.0 * exp(19.0 / 255.0)),
                        (3400.0 * exp(19.0 / 510.0) - 19.0 * pi) * pi /
                            (2108544.0 * exp(19.0 / 255.0)),
                        -pi / (1292.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{
                        (3400.0 * exp(19.0 / 510.0) - 19.0 * pi) * pi /
                            (2108544.0 * exp(19.0 / 255.0)),
                        1.0 + 5.0 * pi / (1292.0 * exp(19.0 / 510.0)) +
                            pi * pi / (73984.0 * exp(19.0 / 255.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),
                    },
                    Vec3{
                        -pi / (1292.0 * exp(19.0 / 510.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),
                        1.0,
                    },
                },
                /* numerically:
                {
                    {0.988342, 0.00479798, -0.00234265},
                    {0.00479798, 1.01184,  0.00351398},
                    {-0.00234265, 0.00351398, 1}
                }
                 */
            },
        };

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    for (const auto& [point, inv_metric] : points_and_inv_metrics) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.inverse_metric(point),
                                          inv_metric, epsilon);
    }
}

void test_christoffel_1() {

    using std::exp;

    constexpr Scalar psi = pi / 8.0;
    constexpr Scalar rho = 17.0;

    const Vec3 point = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0};

    // u

    const Scalar fuuu = 3037500.0 * pi *
                        (26628443.0 * exp(19.0 / 510.0) / 4860000.0 +
                         3170587.0 * pi / 31104000.0) /
                        (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fuuv =
        -1518750.0 *
        (-80523577.0 * exp(19.0 / 510.0) / 2430000.0 - 46303.0 * pi / 7776000) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fuuz =
        -303750.0 *
        (3246473.0 * exp(19.0 / 510.0) / 81000.0 + 130663.0 * pi / 518400.0) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fuvv = 3037500.0 *
                        (306500191.0 * exp(19.0 / 510.0) / 7290000.0 +
                         (4640123.0 * pi) / 46656000.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fuvz = -607500.0 *
                        (-80523577.0 * exp(19.0 / 510.0) / 2430000.0 +
                         130663.0 * pi / 1555200.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fuzz = 3037500.0 *
                        (168835007.0 * exp(19.0 / 510.0) / 12150000.0 +
                         29778377.0 * pi / 388800000.0) *
                        pi / (12165074387 * exp(19.0 / 255.0));

    // v

    const Scalar fvuu = 3037500.0 *
                        (-59093173.0 * exp(19.0 / 510.0) / 1080000.0 +
                         3170587.0 * pi / 46656000.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fvuv = -1012500.0 *
                        (26628443.0 * exp(19.0 / 510.0) / 1620000.0 -
                         46303.0 * pi / 7776000.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fvuz = -607500.0 *
                        (26628443.0 * exp(19.0 / 510.0) / 1620000.0 +
                         130663.0 * pi / 1555200.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fvvv = -3037500.0 *
                        (80523577.0 * exp(19.0 / 510.0) / 4860000.0 -
                         4640123.0 * pi / 69984000.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fvvz =
        202500.0 *
        (3246473.0 * exp(19.0 / 510.0) / 54000.0 - 130663.0 * pi / 777600.0) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const Scalar fvzz = -3037500.0 *
                        (168835007.0 * exp(19.0 / 510.0) / 8100000.0 -
                         29778377.0 * pi / 583200000.0) *
                        pi / (12165074387.0 * exp(19.0 / 255.0));

    // z

    const Scalar fzuu =
        1386695.0 * pi * pi / (81954185344.0 * exp(19.0 / 255.0));

    const Scalar fzuv =
        196295.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const Scalar fzuz = 7311.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const Scalar fzvv =
        1589305.0 * pi * pi / (122931278016.0 * exp(19.0 / 255.0));

    const Scalar fzvz = 2437.0 * pi * pi / (20488546336.0 * exp(19.0 / 255.0));

    const Scalar fzzz =
        357643.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const Ten3x3x3 expected = {
        Mat3x3{
            Vec3{fuuu, fuuv, fuuz},
            Vec3{fuuv, fuvv, fuvz},
            Vec3{fuuz, fuvz, fuzz},
        },
        Mat3x3{
            Vec3{fvuu, fvuv, fvuz},
            Vec3{fvuv, fvvv, fvvz},
            Vec3{fvuz, fvvz, fvzz},
        },
        Mat3x3{
            Vec3{fzuu, fzuv, fzuz},
            Vec3{fzuv, fzvv, fzvz},
            Vec3{fzuz, fzvz, fzzz},
        },
    };

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_step_size,
        ray_step_size * 1e-2,
        ray_step_size * 1e+2,
        ray_step_size * 1e-4,
    };

    const Ten3x3x3 res = geometry.christoffel_1(point);
    for (int i = 0; i < 3; ++i) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(res[i], expected[i], epsilon);
    }
}

void run_test_suite() {
    run(test_ray_passing_through);
    run(test_to_onb_jacobian);
    run(test_from_onb_jacobian);
    run(test_metric);
    run(test_to_cartesian_coords);
    run(test_inverse_metric);
    run(test_christoffel_1);
}

}} // namespace cpp_raytracing::test
