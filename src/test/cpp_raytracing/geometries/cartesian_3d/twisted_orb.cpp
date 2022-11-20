#define BOOST_TEST_MODULE cpp_raytracing::geometry::twisted_orb_cartesian

#include "../../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/geometries/cartesian_3d/twisted_orb/geometry.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

const ray::Scalar epsilon = 1e-12;
const ray::Scalar ray_initial_step_size = 0.1;
const ray::Scalar ray_error_abs = 1e-4;
const ray::Scalar ray_error_rel = 1e-4;
const ray::Scalar ray_max_length = 1e+8;
const ray::Scalar ray_segment_length_factor = 1.1;
const ray::Scalar psi = ray::pi / 8.0;
const ray::Scalar rho = 17.0;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct TwistedOrbGeometryFixture {
    TwistedOrbGeometryFixture() {}
    ~TwistedOrbGeometryFixture() = default;

    ray::cartesian_3d::TwistedOrbGeometry geometry{
        psi,
        rho,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };
};

BOOST_FIXTURE_TEST_CASE(ray_passing_through, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const ray::Vec3 start = {2, -3, 5};
    const ray::Vec3 target = {-7, 11, -13};

    const ray::Vec3 direction_non_normalized = {
        // clang-format off
            -2.0 - sqrt(170.0) * cos(pi / 8 * (exp(-sqrt(38.0) / 17.0) -  exp(-sqrt(339.0) / 17.0)) + atan2(11, 7)) - exp(-sqrt(38.0) / 17.0) * pi / (272.0 * sqrt(38.0)) * (618.0 + 13.0 * sqrt(170.0) * (cos(((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi + 4 * atan2(13483.0, 4956.0)) / 8.0) + sin(((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0 + atan2(11.0, 7.0))))),
            1.0 / (68.0 * sqrt(38.0)) * (exp(-sqrt(38.0) / 17.0) * pi * (-103.0 - 47.0 * cos((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0) + sin((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0)) + 68.0 * (3.0 * sqrt(38.0) + 2.0 * sqrt(1615.0) * sin((exp(-sqrt(38.0) / 17.0) - exp(-sqrt(339.0) / 17.0)) * pi / 8.0 + atan2(11.0, 7.0)))),
            -18.0,
        // clang-format on
    };
    // numericaly: {-8.56156, 14.0917, -18}

    const ray::Vec3 direction =
        geometry.normalize(start, direction_non_normalized);
    // numericaly: {-0.349584, 0.575389, -0.734972}

    const auto ray = geometry.ray_passing_through(start, target);
    BOOST_REQUIRE(ray);

    const auto segment = ray->next_ray_segment();
    BOOST_REQUIRE(segment);

    TEST_EQUAL_RANGES(segment->start(), start);
    TEST_EQUAL_RANGES(segment->direction(), direction);
}

BOOST_FIXTURE_TEST_CASE(to_onb_jacobian, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec3, const ray::Mat3x3>>
        points_and_jacobians = {
            {
                ray::Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                ray::Mat3x3{
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
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(from_onb_jacobian, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec3, const ray::Mat3x3>>
        points_and_jacobians = {
            {
                ray::Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                ray::Mat3x3{
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
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(metric, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec3, const ray::Mat3x3>>
        points_and_metrics = {
            {
                ray::Vec3{2.0, 0.0, 0.0},
                ray::Mat3x3{
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
                ray::Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                ray::Mat3x3{
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
        TEST_EQUAL_RANGES(met, metric);
    }
}

BOOST_FIXTURE_TEST_CASE(to_cartesian_coords, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const ray::Vec3 position = {
        +sqrt(5.0) * cos(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
        -sqrt(5.0) * sin(pi / 8.0 * exp(-sqrt(14.0) / 17.0) - atan(2)),
        3.0,
    };
    // numericaly: {1.57062, 1.59159, 3}
    const ray::Vec3 position_cart = {1.0, 2.0, 3.0};
    TEST_EQUAL_RANGES(geometry.to_cartesian_coords(position), position_cart);
}

BOOST_FIXTURE_TEST_CASE(inverse_metric, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec3, const ray::Mat3x3>>
        points_and_inv_metrics = {
            {
                ray::Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                ray::Mat3x3{
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
        TEST_EQUAL_RANGES(met, inv_metric);
    }
}

BOOST_FIXTURE_TEST_CASE(christoffel_1, TwistedOrbGeometryFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const ray::Vec3 point = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0};

    // u

    const ray::Scalar fuuu = 3037500.0 * pi *
                             (26628443.0 * exp(19.0 / 510.0) / 4860000.0 +
                              3170587.0 * pi / 31104000.0) /
                             (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fuuv =
        -1518750.0 *
        (-80523577.0 * exp(19.0 / 510.0) / 2430000.0 - 46303.0 * pi / 7776000) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fuuz =
        -303750.0 *
        (3246473.0 * exp(19.0 / 510.0) / 81000.0 + 130663.0 * pi / 518400.0) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fuvv = 3037500.0 *
                             (306500191.0 * exp(19.0 / 510.0) / 7290000.0 +
                              (4640123.0 * pi) / 46656000.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fuvz = -607500.0 *
                             (-80523577.0 * exp(19.0 / 510.0) / 2430000.0 +
                              130663.0 * pi / 1555200.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fuzz = 3037500.0 *
                             (168835007.0 * exp(19.0 / 510.0) / 12150000.0 +
                              29778377.0 * pi / 388800000.0) *
                             pi / (12165074387 * exp(19.0 / 255.0));

    // v

    const ray::Scalar fvuu = 3037500.0 *
                             (-59093173.0 * exp(19.0 / 510.0) / 1080000.0 +
                              3170587.0 * pi / 46656000.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fvuv = -1012500.0 *
                             (26628443.0 * exp(19.0 / 510.0) / 1620000.0 -
                              46303.0 * pi / 7776000.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fvuz = -607500.0 *
                             (26628443.0 * exp(19.0 / 510.0) / 1620000.0 +
                              130663.0 * pi / 1555200.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fvvv = -3037500.0 *
                             (80523577.0 * exp(19.0 / 510.0) / 4860000.0 -
                              4640123.0 * pi / 69984000.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fvvz =
        202500.0 *
        (3246473.0 * exp(19.0 / 510.0) / 54000.0 - 130663.0 * pi / 777600.0) *
        pi / (12165074387.0 * exp(19.0 / 255.0));

    const ray::Scalar fvzz = -3037500.0 *
                             (168835007.0 * exp(19.0 / 510.0) / 8100000.0 -
                              29778377.0 * pi / 583200000.0) *
                             pi / (12165074387.0 * exp(19.0 / 255.0));

    // z

    const ray::Scalar fzuu =
        1386695.0 * pi * pi / (81954185344.0 * exp(19.0 / 255.0));

    const ray::Scalar fzuv =
        196295.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const ray::Scalar fzuz =
        7311.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const ray::Scalar fzvv =
        1589305.0 * pi * pi / (122931278016.0 * exp(19.0 / 255.0));

    const ray::Scalar fzvz =
        2437.0 * pi * pi / (20488546336.0 * exp(19.0 / 255.0));

    const ray::Scalar fzzz =
        357643.0 * pi * pi / (40977092672.0 * exp(19.0 / 255.0));

    const ray::Ten3x3x3 expected = {
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

    const ray::Ten3x3x3 chris1 = geometry.christoffel_1(point);
    TEST_EQUAL_RANGES(chris1, expected);
}
