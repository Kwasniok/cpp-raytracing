#include "../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/geometry/twisted_orb_cartesian.hpp>

namespace cpp_raytracing { namespace test {

TEST_CASE("TwistedOrbCartesianGeometry") {

    using std::exp, std::cos, std::sin, std::atan2, std::sqrt;

    const Scalar epsilon = 1e-12;
    const Scalar ray_initial_step_size = 0.1;
    const Scalar ray_error_abs = 1e-4;
    const Scalar ray_error_rel = 1e-4;
    const Scalar ray_max_length = 1e+8;
    const Scalar ray_segment_length_factor = 1.1;
    const Scalar psi = pi / 8.0;
    const Scalar rho = 17.0;

    TwistedOrbCartesianGeometry geometry{
        psi,
        rho,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };

    SUBCASE("ray_passing_through") {

        const Vec3 start = {2, -3, 5};
        const Vec3 target = {-7, 11, -13};

        const Vec3 direction_non_normalized = {
            // clang-format off
            -2.0 - sqrt(170.0) * cos(pi / 8 * (exp(-sqrt(38.0) / 17.0) -  exp(-sqrt(339.0) / 17.0)) + atan2(11, 7)) - exp(-sqrt(38.0) / 17.0) * pi / (272.0 * sqrt(38.0)) * (618.0 + 13.0 * sqrt(170.0) * (cos(((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi + 4 * atan2(13483.0, 4956.0)) / 8.0) + sin(((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0 + atan2(11.0, 7.0))))),
            1.0 / (68.0 * sqrt(38.0)) * (exp(-sqrt(38.0) / 17.0) * pi * (-103.0 - 47.0 * cos((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0) + sin((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0)) + 68.0 * (3.0 * sqrt(38.0) + 2.0 * sqrt(1615.0) * sin((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0 + atan2(11.0, 7.0)))),
            -18.0,
            // clang-format on
        };
        // numericaly: {-8.56156, 14.0917, -18}

        const Vec3 direction =
            geometry.normalize(start, direction_non_normalized);
        // numericaly: {-0.349584, 0.575389, -0.734972}

        const auto ray = geometry.ray_passing_through(start, target);
        REQUIRE(ray);

        const auto segment = ray->next_ray_segment();
        REQUIRE(segment);

        CHECK_ITERABLE_APPROX_EQUAL(epsilon, segment->start(), start);
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, segment->direction(), direction);
    }

    SUBCASE("to_onb_jacobian") {

        const std::vector<std::pair<const Vec3, const Mat3x3>>
            points_and_jacobians = {
                {
                    Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                    Mat3x3{
                        // clang-format off
                        cos(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (5168.0 * exp(19.0 / 510.0)),
                        -sin(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (7752.0 * exp(19.0 / 510.0)),
                        sqrt(13.0) * pi * sin(pi / (8.0 * exp(19.0 / 510.0)) + atan2(2, 3)) / (2584.0 * exp(19.0 / 510.0)),

                        sin(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (5168.0 * exp(19.0 / 510.0)),
                        cos(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (7752.0 * exp(19.0 / 510.0)),
                        -sqrt(13.0) * pi * cos(pi / (8.0 * exp(19.0 / 510.0)) + atan2(2, 3)) / (2584.0 * exp(19.0 / 510.0)),

                        0, 0, 1,
                        // clang-format on
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

        for (const auto& [point, jacobian] : points_and_jacobians) {
            const auto jac = geometry.to_onb_jacobian(point);
            CHECK_ITERABLE_APPROX_EQUAL(epsilon, jac, jacobian);
        }
    }

    SUBCASE("from_onb_jacobian") {

        const std::vector<std::pair<const Vec3, const Mat3x3>>
            points_and_jacobians = {
                {
                    Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                    Mat3x3{
                        // clang-format off
                        cos(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (-3.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 2.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (7752.0 * exp(19.0 / 510.0)),
                        sin(pi / (8.0 * exp(19.0 / 510.0))) -  5.0 * pi * (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (7752.0 * exp(19.0 / 510.0)),
                        -pi / (1292.0 * exp(19.0 / 510.0)),

                        (15.0 * pi * cos(pi / (8.0 * exp(19.0 / 510.0))) - 2.0 * (2584.0 * exp(19.0 / 510.0) + 5 * pi) * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (5168.0 * exp(19.0 / 510.0)),
                        cos(pi / (8.0 * exp(19.0 / 510.0))) + 5.0 * pi * (2.0 * cos(pi / (8.0 * exp(19.0 / 510.0))) + 3.0 * sin(pi / (8.0 * exp(19.0 / 510.0)))) / (5168.0 * exp(19.0 / 510.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),

                        0, 0, 1,
                        // clang-format on
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

        for (const auto& [point, jacobian] : points_and_jacobians) {
            const auto jac = geometry.from_onb_jacobian(point);
            CHECK_ITERABLE_APPROX_EQUAL(epsilon, jac, jacobian);
        }
    }

    SUBCASE("metric") {

        const std::vector<std::pair<const Vec3, const Mat3x3>>
            points_and_metrics = {
                {
                    Vec3{2.0, 0.0, 0.0},
                    Mat3x3{
                        // clang-format off
                        1.0 + pi * pi / (4624.0 * exp(4.0 / 17.0)),
                        -pi / (68.0 * exp(2.0 / 17.0)),
                        0.0,

                        -pi / (68.0 * exp(2.0 / 17.0)),
                        1.0,
                        0.0,

                        0.0, 0.0, 1.0,
                        // clang-format on
                    },
                },
                {
                    Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                    Mat3x3{
                        // clang-format off
                        1.0 + 5.0 * pi * (20672.0 * exp(19.0 / 510.0) + 65.0 * pi) /  (26708224.0 * exp(19.0 / 255.0)),
                        25.0 * pi * (-2584.0 * exp(19.0 / 510) + 13.0 * pi) / (40062336.0 * exp(19.0 / 255.0)),
                        pi * (10336.0 * exp(19.0 / 510.0) + 65.0 * pi) / (13354112.0 * exp(19.0 / 255.0)),

                        25.0 * pi * (-2584 * exp(19.0 / 510) + 13.0 * pi) / (40062336.0 * exp(19.0 / 255.0)),
                        1.0 + 5.0 * pi * (-46512.0 * exp(19.0 / 510.0) + 65.0 * pi) / (60093504.0 * exp(19.0 / 255.0)),
                        pi * (-23256.0 * exp(19.0 / 510.0) + 65.0 * pi) / (20031168.0 * exp(19.0 / 255.0)),

                        pi * (10336.0 * exp(19.0 / 510.0) + 65.0 * pi) / (13354112.0 * exp(19.0 / 255.0)),
                        pi * (-23256.0 * exp(19.0 / 510.0) + 65.0 * pi) / (20031168.0 * exp(19.0 / 255.0)),
                        1.0 + 13 * pi * pi / (6677056.0 * exp(19.0 / 255.0)),
                        // clang-format on
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

        for (const auto& [point, metric] : points_and_metrics) {
            const auto met = geometry.metric(point);
            CHECK_ITERABLE_APPROX_EQUAL(epsilon, met, metric);
        }
    }

    SUBCASE("to_cartesian_coords") {

        const Vec3 position = {
            +sqrt(5.0) * cos(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
            -sqrt(5.0) * sin(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
            3.0,
        };
        // numericaly: {1.57062, 1.59159, 3}
        const Vec3 position_cart = {1.0, 2.0, 3.0};
        CHECK_ITERABLE_APPROX_EQUAL(
            epsilon, geometry.to_cartesian_coords(position), position_cart);
    }

    SUBCASE("inverse_metric") {

        const std::vector<std::pair<const Vec3, const Mat3x3>>
            points_and_inv_metrics = {
                {
                    Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                    Mat3x3{
                        // clang-format off
                        1.0 - 5.0 * pi / (1292.0 * exp(19.0 / 510.0)) + pi * pi / (166464.0 * exp(19.0 / 255.0)),
                        (3400.0 * exp(19.0 / 510.0) - 19.0 * pi) * pi / (2108544.0 * exp(19.0 / 255.0)),
                        -pi / (1292.0 * exp(19.0 / 510.0)),
                        (3400.0 * exp(19.0 / 510.0) - 19.0 * pi) * pi / (2108544.0 * exp(19.0 / 255.0)),
                        1.0 + 5.0 * pi / (1292.0 * exp(19.0 / 510.0)) +  pi * pi / (73984.0 * exp(19.0 / 255.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),

                        -pi / (1292.0 * exp(19.0 / 510.0)),
                        3.0 * pi / (2584.0 * exp(19.0 / 510.0)),
                        1.0,
                        // clang-format on
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

        for (const auto& [point, inv_metric] : points_and_inv_metrics) {
            const auto met = geometry.inverse_metric(point);
            CHECK_ITERABLE_APPROX_EQUAL(epsilon, met, inv_metric);
        }
    }

    SUBCASE("christoffel_1") {

        const Vec3 point = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0};

        // u

        const Scalar fuuu = 3037500.0 * pi *
                            (26628443.0 * exp(19.0 / 510.0) / 4860000.0 +
                             3170587.0 * pi / 31104000.0) /
                            (12165074387.0 * exp(19.0 / 255.0));

        const Scalar fuuv = -1518750.0 *
                            (-80523577.0 * exp(19.0 / 510.0) / 2430000.0 -
                             46303.0 * pi / 7776000) *
                            pi / (12165074387.0 * exp(19.0 / 255.0));

        const Scalar fuuz = -303750.0 *
                            (3246473.0 * exp(19.0 / 510.0) / 81000.0 +
                             130663.0 * pi / 518400.0) *
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

        const Scalar fvvz = 202500.0 *
                            (3246473.0 * exp(19.0 / 510.0) / 54000.0 -
                             130663.0 * pi / 777600.0) *
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

        const Scalar fzuz =
            7311.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

        const Scalar fzvv =
            1589305.0 * pi * pi / (122931278016.0 * exp(19.0 / 255.0));

        const Scalar fzvz =
            2437.0 * pi * pi / (20488546336.0 * exp(19.0 / 255.0));

        const Scalar fzzz =
            357643.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

        const Ten3x3x3 expected = {
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

        const Ten3x3x3 chris1 = geometry.christoffel_1(point);
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, chris1, expected);
    }
}

}} // namespace cpp_raytracing::test
