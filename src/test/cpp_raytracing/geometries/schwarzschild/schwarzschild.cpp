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
                    (10 * sqrt(293.0 / 323.0)) / 19.0,
                    (06 * sqrt(293.0 / 323.0)) / 19.0,
                    0.0,

                    18.0 / (19.0 * sqrt(13.0)),
                    12.0 / (19.0 * sqrt(13.0)),
                    -5.0 * sqrt(13.0) / 19.0,
                    0.0,

                    -2.0 / sqrt(13.0),
                    3.0 / sqrt(13.0),
                    0.0,
                    0.0,
                },
                /* numerically:
        {
            {0.7519175250616986, 0.5012783500411324, 0.3007670100246794, 0},
            {0.2627527245277401, 0.1751684830184934, -0.9488292830168393, 0},
            {-0.5547001962252291, 0.8320502943378437, 0, 0}
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
                    18.0 / (19.0 * sqrt(13.0)),
                    -2 / sqrt(13),

                    10.0 * sqrt(17.0 / 5567.0),
                    12.0 / (19.0 * sqrt(13.0)),
                    3 / sqrt(13),

                    6.0 * sqrt(17.0 / 5567.0),
                    -5.0 * sqrt(13.0) / 19.0,
                    0,

                    0,
                    0,
                    0,
                },
                /* numerically:
        {
            {0.8289056675594834, 0.2627527245277401, -0.5547001962252291},
            {0.5526037783729889, 0.1751684830184934, 0.8320502943378437},
            {0.3315622670237934, -0.9488292830168393, 0},
            {0, 0, 0}
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

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct SchwarzschildFlatFixture {
    SchwarzschildFlatFixture() {}
    ~SchwarzschildFlatFixture() = default;

    ray::schwarzschild::Geometry geometry{
        1.0,
        0.0,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };
};

BOOST_FIXTURE_TEST_CASE(jacobian_relations_flat, SchwarzschildFlatFixture,
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

BOOST_FIXTURE_TEST_CASE(metric_flat, SchwarzschildFlatFixture,
                        *but::tolerance(epsilon)) {
    using std::pow, std::sin;

    const std::vector<std::pair<const ray::Vec<4_D>, const ray::Mat<4_D>>>
        points_and_metrics = {
            {
                ray::Vec<4_D>{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0},
                ray::Mat<4_D>{ray::Vec<4_D>{1.0, 0.0, 0.0, 0.0},
                              ray::Vec<4_D>{0.0, 1.0, 0.0, 0.0},
                              ray::Vec<4_D>{0.0, 0.0, 1.0, 0.0},
                              ray::Vec<4_D>{0.0, 0.0, 0.0, -1.0}},
            },
        };

    for (const auto& [point, metric] : points_and_metrics) {
        const auto met = geometry.metric(point);
        TEST_EQUAL_RANGES(met, metric);
    }
}

BOOST_FIXTURE_TEST_CASE(christoffel_2_flat, SchwarzschildFlatFixture,
                        *but::tolerance(epsilon)) {
    using std::exp, std::cos, std::sin, std::atan2, std::sqrt, ray::pi;

    const ray::Vec<4_D> point = {1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0, 1.0 / 7.0};
    const ray::TenR3<4_D> expected = {};

    const ray::TenR3<4_D> chris2 = geometry.christoffel_2(point);
    TEST_EQUAL_RANGES(chris2, expected);
}