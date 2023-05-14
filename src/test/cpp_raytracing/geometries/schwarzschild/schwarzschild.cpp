#define BOOST_TEST_MODULE cpp_raytracing::geometry::schwarzschild

#include "../../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/geometries/schwarzschild/geometry.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

const ray::Scalar epsilon = 1e-12;
const ray::Scalar ray_initial_step_size = 0.1;
const ray::Scalar ray_error_abs = 1e-4;
const ray::Scalar ray_error_rel = 1e-4;
const ray::Scalar ray_max_length = 1e+8;
const ray::Scalar ray_segment_length_factor = 1.1;
const ray::Scalar speed_of_light = 11.0;
const ray::Scalar schwarzschild_radius = 1.0 / 17.0;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct SchwarzschildFixture {
    SchwarzschildFixture() {}
    ~SchwarzschildFixture() = default;

    ray::schwarzschild::Geometry geometry{
        speed_of_light,
        schwarzschild_radius,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };
};

BOOST_FIXTURE_TEST_CASE(to_onb_jacobian, SchwarzschildFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec<4_D>, const ray::Mat<3_D, 4_D>>>
        points_and_jacobians = {
            {
                ray::Vec<4_D>{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0},
                ray::Mat<3_D, 4_D>{
                    (15 * sqrt(293.0 / 323.0)) / 19.0,
                    (3 * sqrt(293.0 / 4199.0)) / 5.0,
                    -sqrt((293.0 / 323.0)) / 3.0,
                    0.0,

                    300.0 / 361.0,
                    12.0 / (19.0 * sqrt(13.0)),
                    15.0 / 19.0,
                    0.0,

                    36.0 / (19.0 * sqrt(13.0)),
                    -1.0,
                    0.0,
                    0.0,
                },
                /* numerically:
        {
            {0.7519175250616986, 0.1584937435048269, -0.3174762883593838, 0},
            {0.8310249307479224, 0.1751684830184934, 0.7894736842105263, 0},
            {0.5255054490554802, -1.000000000000000, 0, 0}
        }
                 */
            },
        };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        const auto jac = geometry.to_onb_jacobian(point);
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(from_onb_jacobian, SchwarzschildFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const std::vector<std::pair<const ray::Vec<4_D>, const ray::Mat<4_D, 3_D>>>
        points_and_jacobians = {
            {
                ray::Vec<4_D>{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0},
                ray::Mat<4_D, 3_D>{
                    15.0 * sqrt(17.0 / 5567.0),
                    1.0 / 3.0,
                    sqrt(13.0) / 19.0,

                    (540.0 * sqrt(17.0 / 72371.0)) / 19.0,
                    12.0 / (19.0 * sqrt(13.0)),
                    -325.0 / 361,

                    -(12.0 * sqrt(323.0 / 293.0)) / 13.0,
                    57.0 / 65.0,
                    0,

                    0,
                    0,
                    0,
                },
                /* numerically:
        {
            {0.8289056675594834, 0.3333333333333333, 0.1897658566033679},
            {0.4355944450554789, 0.1751684830184934, -0.9002770083102493},
            {-0.9691820113003190, 0.8769230769230769, 0}, {0, 0, 0},
        }
                 */
            },
        };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        const auto jac = geometry.from_onb_jacobian(point);
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(jacobian_relations, SchwarzschildFixture,
                        *but::tolerance(epsilon)) {
    using namespace ray::tensor;

    const std::vector<ray::Vec<4_D>> points = {
        ray::Vec<4_D>{2.0, 3.0, 5.0, 7.0},
        ray::Vec<4_D>{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0},
    };

    for (const auto& point : points) {
        const auto to_jac = geometry.to_onb_jacobian(point);
        const auto from_jac = geometry.from_onb_jacobian(point);

        {
            // to * from
            const ray::Mat<3_D> res = to_jac * from_jac;
            const auto& expected = identity_mat<3_D>;
            TEST_EQUAL_RANGES(res, expected);
        }
        {
            // from * to
            const ray::Mat<4_D> res = from_jac * to_jac;
            const ray::Mat<4_D> expected = {
                // clang-format off
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 0,
                // clang-format on
            };
            TEST_EQUAL_RANGES(res, expected);
        }
    }
}

BOOST_FIXTURE_TEST_CASE(metric, SchwarzschildFixture,
                        *but::tolerance(epsilon)) {
    using std::pow, std::sin;

    const std::vector<std::pair<const ray::Vec<4_D>, const ray::Mat<4_D>>>
        points_and_metrics = {
            {
                ray::Vec<4_D>{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0},
                ray::Mat<4_D>{
                    ray::Vec<4_D>{112523.0 / 105773.0, 4500.0 / 105773.0,
                                  2700.0 / 105773.0, 0.0},
                    ray::Vec<4_D>{4500.0 / 105773.0, 108773.0 / 105773.0,
                                  1800.0 / 105773.0, 0.0},
                    ray::Vec<4_D>{2700.0 / 105773.0, 1800.0 / 105773.0,
                                  106853.0 / 105773.0, 0.0},
                    ray::Vec<4_D>{0.0, 0.0, 0.0,
                                  -((293.0 * pow(speed_of_light, 2)) / 323.0)}},
                /* numerically:
                {
                    {-2.000000000000000, 0, 0, 0},
                    {0, 0.1111111111111111, 0, 0},
                    {0, 0, 0.004385500333173051, 0},
                    {0, 0, 0, 0.5000000000000000 * c^2},
                }
                 */
            },
        };

    for (const auto& [point, metric] : points_and_metrics) {
        const auto met = geometry.metric(point);
        TEST_EQUAL_RANGES(met, metric);
    }
}

BOOST_FIXTURE_TEST_CASE(internal_spherical_swarzschild_christoffel_2,
                        SchwarzschildFixture, *but::tolerance(epsilon)) {
    using std::sqrt, ray::pi;

    const ray::Scalar r = 2.0;
    const ray::Scalar theta = pi / 3.0;

    const ray::TenR3<4_D> expected = {
        // clang-format off
            -1.0 / 132.0, 0, 0, 0,
            0, -33.0 / 17.0, 0, 0,
            0, 0, -99.0 / 68.0, 0,
            0, 0, 0, 3993.0 / 4624.0,

            0, 1.0 / 2.0, 0, 0,
            1.0 / 2.0, 0, 0, 0,
            0, 0, -sqrt(3) / 4.0, 0,
            0, 0, 0, 0,

            0, 0, 1.0 / 2.0, 0,
            0, 0, 1.0 / sqrt(3), 0,
            1.0 / 2.0, 1.0 / sqrt(3), 0, 0,
            0, 0, 0, 0,

            0, 0, 0, 1.0 / 132.0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            1.0 / 132.0, 0, 0, 0,
        // clang-format on
    };
    /* numerically:
        {
            {
                {-0.007575757575757576, 0, 0, 0},
                {0, -1.941176470588235, 0, 0},
                {0, 0, -1.455882352941176, 0},
                {0, 0, 0, 0.8635380622837370}
            },
            {
                {0, 0.5000000000000000, 0, 0},
                {0.5000000000000000, 0, 0, 0},
                {0, 0, -0.4330127018922193, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0.5000000000000000, 0},
                {0, 0, 0.5773502691896258, 0},
                {0.5000000000000000, 0.5773502691896258, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0.007575757575757576},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0.007575757575757576, 0, 0, 0}
            }
        }
        */

    const ray::TenR3<4_D> chris2 =
        ray::schwarzschild::_spherical_swarzschild_christoffel_2(
            speed_of_light, schwarzschild_radius, r, theta);
    TEST_EQUAL_RANGES(chris2, expected);
}

BOOST_AUTO_TEST_CASE(
    internal_cartesian_from_spherical_swarzschild_contravariant_jacobian,
    *but::tolerance(epsilon)) {
    using std::sqrt, ray::pi;

    const ray::Scalar r = 2.0;
    const ray::Scalar theta = pi / 3.0;
    const ray::Scalar phi = pi / 5.0;

    const ray::Mat<4_D> expected = {
        // clang-format off
        1.0 / 8.0 * sqrt(3.0) * (1.0 + sqrt(5.0)),
        1.0 / 4.0 * sqrt(3.0 / 2.0 * (5.0 - sqrt(5.0))),
        1.0 / 2.0,
        0,

        1.0 / 16.0 * (1.0 + sqrt(5.0)),
        1.0 / 8.0 * sqrt(1.0 / 2.0 * (5.0 - sqrt(5.0))),
        -sqrt(3.0) / 4.0,
        0,

        -(1.0 / 2.0) * sqrt(1.0 / 6.0 * (5.0 - sqrt(5.0))),
        (1.0 + sqrt(5.0)) / (4.0 * sqrt(3.0)),
        0,
        0,

        0, 0, 0, 1,
        // clang-format on
    };
    /* numerically:
        {
            {0.7006292692220368, 0.5090369604551272, 0.5000000000000000, 0},
            {0.2022542485937369, 0.1469463130731183, -0.4330127018922193, 0},
            {-0.3393579736367515, 0.4670861794813578, 0, 0},
            {0, 0, 0, 1.000000000000000}
        }
     */

    const ray::Mat<4_D> jacobian = ray::schwarzschild::
        _cartesian_from_spherical_swarzschild_contravariant_jacobian(r, theta,
                                                                     phi);
    TEST_EQUAL_RANGES(jacobian, expected);
}

BOOST_AUTO_TEST_CASE(
    internal_cartesian_from_spherical_swarzschild_covariant_jacobian,
    *but::tolerance(epsilon)) {
    using std::sqrt, ray::pi;

    const ray::Scalar r = 2.0;
    const ray::Scalar theta = pi / 3.0;
    const ray::Scalar phi = pi / 5.0;

    const ray::Mat<4_D> expected = {
        // clang-format off
        1.0 / 8.0 * sqrt(3.0) * (1.0 + sqrt(5.0)),
        1.0 / 4.0 * (1.0 + sqrt(5.0)),
        -1.0 / 2.0 * sqrt(3.0 / 2.0 * (5.0 - sqrt(5.0))),
        0,

        1.0 / 4.0 * sqrt(3.0 / 2.0 * (5.0 - sqrt(5.0))),
        sqrt(5.0 / 8.0 - sqrt(5.0) / 8.0),
        1.0 / 4.0 * sqrt(3.0) * (1.0 + sqrt(5.0)),
        0,

        1.0 / 2.0,
        -sqrt(3.0),
        0,
        0,

        0, 0, 0, 1,
        // clang-format on
    };
    /* numerically:
        {
            {0.7006292692220368, 0.8090169943749474, -1.018073920910254, 0},
            {0.5090369604551272, 0.5877852522924731, 1.401258538444074, 0},
            {0.5000000000000000, -1.732050807568877, 0, 0},
            {0, 0, 0, 1.000000000000000}
        }
     */

    const ray::Mat<4_D> jacobian = ray::schwarzschild::
        _cartesian_from_spherical_swarzschild_covariant_jacobian(r, theta, phi);
    TEST_EQUAL_RANGES(jacobian, expected);
}

BOOST_FIXTURE_TEST_CASE(christoffel_2, SchwarzschildFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const ray::Vec<4_D> point = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0};

    const ray::TenR3<4_D> expected = {
        // clang-format off
        2787750.0 / 725497007.0,
        -920362500.0 / 12333449119.0,
        -552217500.0 / 12333449119.0,
        0.0,

        -920362500.0 / 12333449119.0,
        814360500.0 / 12333449119.0,
        -368145000.0 / 12333449119.0,
        0.0,

        -552217500.0 / 12333449119.0,
        -368145000.0 / 12333449119.0,
        1207048500.0 / 12333449119.0,
        0.0,

        0.0,
        0.0,
        0.0,
        239307750.0 / 37662769.0,

        1858500.0 / 725497007.0,
        -613575000.0 / 12333449119.0,
        -368145000.0 / 12333449119.0,
        0.0,

        -613575000.0 / 12333449119.0,
        542907000.0 / 12333449119.0,
        -245430000.0 / 12333449119.0,
        0.0,

        -368145000.0 / 12333449119.0,
        -245430000.0 / 12333449119.0,
        804699000.0 / 12333449119.0,
        0.0,

        0.0,
        0.0,
        0.0,
        159538500.0 / 37662769.0,

        1115100.0 / 725497007.0,
        -368145000.0 / 12333449119.0,
        -220887000.0 / 12333449119.0,
        0.0,

        -368145000.0 / 12333449119.0,
        325744200.0 / 12333449119.0,
        -147258000.0 / 12333449119.0,
        0.0,

        -220887000.0 / 12333449119.0,
        -147258000.0 / 12333449119.0,
        482819400.0 / 12333449119.0,
        0.0,

        0.0,
        0.0,
        0.0,
        95723100.0 / 37662769.0,

        0.0,
        0.0,
        0.0,
        6750.0 / 105773.0,

        0.0,
        0.0,
        0.0,
        4500.0 / 105773.0,

        0.0,
        0.0,
        0.0,
        2700.0 / 105773.0,

        6750.0 / 105773.0,
        4500.0 / 105773.0,
        2700.0 / 105773.0,
        0.0,
        // clang-format on
    };

    const ray::TenR3<4_D> chris2 = geometry.christoffel_2(point);
    TEST_EQUAL_RANGES(chris2, expected);
}