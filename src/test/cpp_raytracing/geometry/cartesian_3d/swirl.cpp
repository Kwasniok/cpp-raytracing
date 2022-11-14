#define BOOST_TEST_MODULE cpp_raytracing::geometry::swirl_cartesian

#include "../../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/geometry/cartesian_3d/swirl.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

const ray::Scalar epsilon = 1e-12;
const ray::Scalar ray_initial_step_size = 0.1;
const ray::Scalar ray_error_abs = 1e-4;
const ray::Scalar ray_error_rel = 1e-4;
const ray::Scalar ray_max_length = 1e+8;
const ray::Scalar ray_segment_length_factor = 1.1;
const ray::Scalar a = 1.0 / 17.0;

struct SwirlCartesianGeometry3DFixture {
    SwirlCartesianGeometry3DFixture() {}
    ~SwirlCartesianGeometry3DFixture() = default;

    ray::SwirlCartesianGeometry3D geometry{
        a,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };
};

BOOST_FIXTURE_TEST_CASE(ray_passing_through, SwirlCartesianGeometry3DFixture,
                        *but::tolerance(epsilon)) {
    using std::sqrt, std::cos, std::sin, std::atan, std::atan2;

    const ray::Vec3 start = {2.0, 3.0, 5.0};
    const ray::Vec3 target = {7.0, 11.0, 13.0};

    const ray::Vec3 direction_non_normalized = {
        // clang-format off
            -2.0 + 9.0 * sqrt(13) / 17.0 + sqrt(10.0 / 17.0) * (15.0 * cos(sqrt(65.0 * (447.0 - 2.0 * sqrt(2210))) / 17.0 + atan(1.0 / 47.0)) + 17.0 * cos(sqrt(65.0 * (447.0 - 2.0 * sqrt(2210))) / 17.0 + atan2(11, 7))),
            -3.0 - 6.0 * sqrt(13.0) / 17.0 - 5.0 * sqrt(65.0 / 34.0) * (cos(13.0 * sqrt(10.0 / 17.0) - 5.0 * sqrt(13.0) / 17.0 + atan(11.0 / 7.0)) + cos(13.0 * sqrt(10.0 / 17.0) - 5.0 * sqrt(13.0) / 17.0 - 2.0 * atan2(3, 2) + atan(11.0 / 7.0))) + sqrt(170.0) * sin(13.0 * sqrt(10.0 / 17.0) - 5.0 * sqrt(13) / 17.0 + atan2(11.0, 7.0)),
            +8.0,
        // clang-format on
    };
    /*
     * numerical:
     * {-21.7314, -3.64678, 8}
     */
    const ray::Vec3 direction =
        geometry.normalize(start, direction_non_normalized);

    const auto ray = geometry.ray_passing_through(start, target);
    BOOST_REQUIRE(ray);

    const auto segment = ray->next_ray_segment();
    BOOST_REQUIRE(segment);

    TEST_EQUAL_RANGES(segment->start(), start);
    TEST_EQUAL_RANGES(segment->direction(), direction);
}

BOOST_FIXTURE_TEST_CASE(to_onb_jacobian, SwirlCartesianGeometry3DFixture,
                        *but::tolerance(epsilon)) {
    using std::sqrt, std::cos, std::sin, std::atan, std::atan2;

    const ray::Vec3 point = {2.0, 3.0, 5.0};

    const ray::Mat3x3 to_onb_jacobian = {
        // clang-format off
            ((2.0 + 15.0 * sqrt(13.0) / 17.0) * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) + 3.0 * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            ((2.0 + 15.0 * sqrt(13.0) / 17.0) * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) - 3.0 * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            3.0 * sqrt(13.0) / 17.0,

            ((3.0 - 10.0 * sqrt(13.0) / 17.0) * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) - 2.0 * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            ((3.0 - 10.0 * sqrt(13.0) / 17.0) * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) + 2.0 * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            -2.0 * sqrt(13.0) / 17.0,
            
            0.0, 0.0, 1.0,
        // clang-format on
    };
    /*
     * numerical:
     * {
     *     {0.0869394, -0.604889, 0},
     *     {1.65827, -0.0353227, 0},
     *     {0.636274, -0.424183, 1},
     * }
     */

    const auto jacobian = geometry.to_onb_jacobian(point);
    TEST_EQUAL_RANGES(jacobian, to_onb_jacobian);
}

BOOST_FIXTURE_TEST_CASE(from_onb_jacobian, SwirlCartesianGeometry3DFixture,
                        *but::tolerance(epsilon)) {
    using std::sqrt, std::cos, std::sin, std::atan, std::atan2;

    const ray::Vec3 point = {2.0, 3.0, 5.0};

    const ray::Mat3x3 from_onb_jacobian = {
        // clang-format off
            ((3.0 - 10.0 * sqrt(13.0) / 17.0) * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) +  2.0 * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            (-(2.0 + 15.0 * sqrt(13.0) / 17.0) * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) + 3.0 * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            -13.0 / 17.0 * sin(5.0 * sqrt(13.0) / 17.0 + atan2(3, 2)),

            (-(3.0 - 10.0 * sqrt(13.0) / 17.0) * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) + 2.0 * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            ((2.0 + 15.0 * sqrt(13.0) / 17.0) * cos(5.0 * sqrt(13) / 17.0 + atan2(3, 2)) + 3.0 * sin(5.0 * sqrt(13) / 17.0 + atan2(3, 2))) / sqrt(13.0),
            13.0 / 17.0 * cos(5.0 * sqrt(13.0) / 17.0 + atan2(3, 2)),

           0.0, 0.0, 1.0,
        // clang-format on
    };
    /*
     * numerical:
     * {
     *     {-0.0353227, 0.604889, 0},
     *     {-1.65827, 0.0869394, 0},
     *     {-0.680936, -0.347997, 1},
     * }
     */

    const auto jacobian = geometry.from_onb_jacobian(point);
    TEST_EQUAL_RANGES(jacobian, from_onb_jacobian);
}

BOOST_FIXTURE_TEST_CASE(metric, SwirlCartesianGeometry3DFixture,
                        *but::tolerance(epsilon)) {
    using std::sqrt, std::cos, std::sin, std::atan, std::atan2;

    using Pair = std::pair<const ray::Vec3, const ray::Mat3x3>;
    const std::vector<Pair> points_and_metrics = {
        {
            ray::Vec3{2.0, 0.0, 0.0},
            ray::Mat3x3{
                // clang-format off
                    1.0, 0.0, 0.0,
                    0.0, 1.0, 4 * a,
                    0.0, 4 * a, 1 + 16 * a * a,
                // clang-format on
            },
        },
        {
            ray::Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
            /*
             * numerical:
             * {0.5, 0.333333, 0.2}
             */
            ray::Mat3x3{
                // clang-format off
                    28901.0 / 28900.0 - 2.0 / (85.0 * sqrt(13.0)),
                    1.0 / 43350.0 + 1.0 / (102 * sqrt(13.0)),
                    (13.0 - 340.0 * sqrt(13.0)) / 104040,

                    1.0 / 43350.0 + 1.0 / (102.0 * sqrt(13.0)),
                    65026.0 / 65025.0 + 2.0 / (85.0 * sqrt(13.0)),
                    (13.0 + 765.0 * sqrt(13.0)) / 156060.0,

                    (13.0 - 340.0 * sqrt(13.0)) / 104040.0,
                    (13.0 + 765.0 * sqrt(13.0)) / 156060.0,
                    374713.0 / 374544.0,
                // clang-format on
            },
            /*
             * numerical:
             * {
             *     {0.993509, 0.00274219, -0.0116579},
             *     {0.00274219, 1.00654, 0.0177576},
             *     {-0.0116579, 0.0177576, 1.00045},
             * }
             */
        },
    };

    for (const auto& [point, metric] : points_and_metrics) {
        const auto met = geometry.metric(point);
        TEST_EQUAL_RANGES(met, metric);
    }
}
