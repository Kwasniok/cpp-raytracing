#include <vector>

#include <cpp_raytracing/geometry/swirl_cartesian.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1e-12;
constexpr Scalar ray_initial_step_size = 0.1;
constexpr Scalar ray_error_abs = 1e-4;
constexpr Scalar ray_error_rel = 1e-4;
constexpr Scalar ray_max_length = 1e+8;
constexpr Scalar ray_segment_length_factor = 1.1;

void test_ray_passing_through() {

    constexpr Scalar a = 1.0 / 17.0;

    const Vec3 start = {2.0, 3.0, 5.0};
    const Vec3 target = {7.0, 11.0, 13.0};

    const Vec3 direction = {
        -2.0 + 9.0 * std::sqrt(13) / 17.0 +
            std::sqrt(10.0 / 17.0) *
                (15.0 * std::cos(
                            std::sqrt(65.0 * (447.0 - 2.0 * std::sqrt(2210))) /
                                17.0 +
                            std::atan(1.0 / 47.0))

                 + 17.0 * std::cos(std::sqrt(65.0 *
                                             (447.0 - 2.0 * std::sqrt(2210))) /
                                       17.0 +
                                   std::atan2(11, 7))),

        -3.0 - 6.0 * std::sqrt(13.0) / 17.0 -
            5.0 * std::sqrt(65.0 / 34.0) *
                (std::cos(13.0 * std::sqrt(10.0 / 17.0) -
                          5.0 * std::sqrt(13.0) / 17.0 +
                          std::atan(11.0 / 7.0)) +
                 std::cos(13.0 * std::sqrt(10.0 / 17.0) -
                          5.0 * std::sqrt(13.0) / 17.0 -
                          2.0 * std::atan2(3, 2) + std::atan(11.0 / 7.0))) +
            std::sqrt(170.0) *
                std::sin(13.0 * std::sqrt(10.0 / 17.0) -
                         5.0 * std::sqrt(13) / 17.0 + std::atan2(11.0, 7.0)),

        8.0,
    };
    /*
     * numerical:
     * {-21.7314, -3.64678, 8}
     */

    SwirlCartesianGeometry geometry{
        a,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };

    const Vec3 normalized_direction = geometry.normalize(start, direction);

    auto ray = geometry.ray_passing_through(start, target);
    TEST_ASSERT_TRUE(ray);
    auto segment = ray->next_ray_segment();
    TEST_ASSERT_TRUE(segment);

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(segment->start(), start, epsilon);
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(segment->direction(),
                                      normalized_direction, epsilon);
}

void test_to_onb_jacobian() {

    constexpr Scalar a = 1.0 / 17.0;

    const Vec3 point = {2.0, 3.0, 5.0};

    const Mat3x3 to_onb_jacobian = {
        Vec3{
            ((2.0 + 15.0 * std::sqrt(13.0) / 17.0) *
                 std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             3.0 * std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            ((2.0 + 15.0 * std::sqrt(13.0) / 17.0) *
                 std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) -
             3.0 * std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            3.0 * std::sqrt(13.0) / 17.0,
        },
        Vec3{
            ((3.0 - 10.0 * std::sqrt(13.0) / 17.0) *
                 std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) -
             2.0 * std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            ((3.0 - 10.0 * std::sqrt(13.0) / 17.0) *
                 std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             2.0 * std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            -2.0 * std::sqrt(13.0) / 17.0,
        },
        Vec3{
            0.0,
            0.0,
            1.0,
        },
    };
    /*
     * numerical:
     * {
     *     {0.0869394, -0.604889, 0},
     *     {1.65827, -0.0353227, 0},
     *     {0.636274, -0.424183, 1},
     * }
     */

    SwirlCartesianGeometry geometry{
        a,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };

    // to_onb_jacobiand
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.to_onb_jacobian(point),
                                      to_onb_jacobian, epsilon);
}

void test_from_onb_jacobian() {

    constexpr Scalar a = 1.0 / 17.0;

    const Vec3 point = {2.0, 3.0, 5.0};

    const Mat3x3 from_onb_jacobian = {
        Vec3{
            ((3.0 - 10.0 * std::sqrt(13.0) / 17.0) *
                 std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             2.0 * std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            (-(2.0 + 15.0 * std::sqrt(13.0) / 17.0) *
                 std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             3.0 * std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            -13.0 / 17.0 *
                std::sin(5.0 * std::sqrt(13.0) / 17.0 + std::atan2(3, 2)),
        },
        Vec3{
            (-(3.0 - 10.0 * std::sqrt(13.0) / 17.0) *
                 std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             2.0 * std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            ((2.0 + 15.0 * std::sqrt(13.0) / 17.0) *
                 std::cos(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2)) +
             3.0 * std::sin(5.0 * std::sqrt(13) / 17.0 + std::atan2(3, 2))) /
                std::sqrt(13.0),
            13.0 / 17.0 *
                std::cos(5.0 * std::sqrt(13.0) / 17.0 + std::atan2(3, 2)),
        },
        Vec3{
            0.0,
            0.0,
            1.0,
        },
    };
    /*
     * numerical:
     * {
     *     {-0.0353227, 0.604889, 0},
     *     {-1.65827, 0.0869394, 0},
     *     {-0.680936, -0.347997, 1},
     * }
     */

    SwirlCartesianGeometry geometry{
        a,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };

    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.from_onb_jacobian(point),
                                      from_onb_jacobian, epsilon);
}

void test_metric() {

    constexpr Scalar a = 1.0 / 17.0;

    const std::vector<std::pair<const Vec3, const Mat3x3>> points_and_metrics =
        {
            {
                Vec3{2.0, 0.0, 0.0},
                Mat3x3{
                    Vec3{1.0, 0.0, 0.0},
                    Vec3{0.0, 1.0, 4 * a},
                    Vec3{0.0, 4 * a, 1 + 16 * a * a},
                },
            },
            {
                Vec3{1.0 / 2.0, 1.0 / 3.0, 1.0 / 5.0},
                /*
                 * numerical:
                 * {0.5, 0.333333, 0.2}
                 */
                Mat3x3{
                    Vec3{
                        28901.0 / 28900.0 - 2.0 / (85.0 * std::sqrt(13.0)),
                        1.0 / 43350.0 + 1.0 / (102 * std::sqrt(13.0)),
                        (13.0 - 340.0 * std::sqrt(13.0)) / 104040,
                    },
                    Vec3{
                        1.0 / 43350.0 + 1.0 / (102.0 * std::sqrt(13.0)),
                        65026.0 / 65025.0 + 2.0 / (85.0 * std::sqrt(13.0)),
                        (13.0 + 765.0 * std::sqrt(13.0)) / 156060.0,
                    },
                    Vec3{
                        (13.0 - 340.0 * std::sqrt(13.0)) / 104040.0,
                        (13.0 + 765.0 * std::sqrt(13.0)) / 156060.0,
                        374713.0 / 374544.0,
                    },
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

    SwirlCartesianGeometry geometry{
        a,
        ray_initial_step_size,
        ray_error_abs,
        ray_error_rel,
        ray_max_length,
        ray_segment_length_factor,
    };

    for (const auto& [point, metric] : points_and_metrics) {
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(geometry.metric(point), metric,
                                          epsilon);
    }
}

void run_test_suite() {
    run(test_ray_passing_through);
    run(test_to_onb_jacobian);
    run(test_from_onb_jacobian);
    run(test_metric);
}

}} // namespace cpp_raytracing::test
