#define BOOST_TEST_MODULE                                                      \
    cpp_raytracing::geometry::cartesian_embedded::euclidean

#include "../../../common.hpp"

#include <utility>
#include <vector>

#include <cpp_raytracing/geometry/cartesian_embedded/euclidean.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

const ray::Scalar epsilon = 1e-12;

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct CartesianEmbeddedEuclideanGeometry5DFixture {
    CartesianEmbeddedEuclideanGeometry5DFixture() {}
    ~CartesianEmbeddedEuclideanGeometry5DFixture() = default;

    ray::CartesianEmbeddedEuclideanGeometry<5_D> geometry{};
};

BOOST_FIXTURE_TEST_CASE(ray_passing_through,
                        CartesianEmbeddedEuclideanGeometry5DFixture,
                        *but::tolerance(epsilon)) {
    using namespace ray::tensor;

    const ray::Vec<5_D> start = {2, -3, 5, -7, 11};
    const ray::Vec<5_D> target = {13, 17, 19, 23, 27};
    const ray::Vec<5_D> direction_non_normalized = target - start;
    const ray::Vec<5_D> direction = unit_vector(direction_non_normalized);

    const auto ray = geometry.ray_passing_through(start, target);
    BOOST_REQUIRE(ray);

    const auto segment = ray->next_ray_segment();
    BOOST_REQUIRE(segment);

    TEST_EQUAL_RANGES(segment->start(), start);
    TEST_EQUAL_RANGES(segment->direction(), direction);
}

BOOST_FIXTURE_TEST_CASE(to_onb_jacobian,
                        CartesianEmbeddedEuclideanGeometry5DFixture,
                        *but::tolerance(epsilon)) {
    using namespace ray::tensor;

    const std::vector<std::pair<const ray::Vec<5_D>, const ray::Mat<3_D, 5_D>>>
        points_and_jacobians = {
            {
                ray::Vec<5_D>{1.0, 2.0, 3.0, 4.0, 5.0},
                ray::Mat<3_D, 5_D>{
                    // clang-format off
                    1, 0, 0, 0, 0,
                    0, 1, 0, 0, 0,
                    0, 0, 1, 0, 0,
                    // clang-format on
                },
            },
        };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        const auto jac = geometry.to_onb_jacobian(point);
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(from_onb_jacobian,
                        CartesianEmbeddedEuclideanGeometry5DFixture,
                        *but::tolerance(epsilon)) {
    using namespace ray::tensor;

    const std::vector<std::pair<const ray::Vec<5_D>, const ray::Mat<5_D, 3_D>>>
        points_and_jacobians = {
            {
                ray::Vec<5_D>{1.0, 2.0, 3.0, 4.0, 5.0},
                ray::Mat<5_D, 3_D>{
                    // clang-format off
                    1, 0, 0,
                    0, 1, 0,
                    0, 0, 1,
                    0, 0, 0,
                    0, 0, 0,
                    // clang-format on
                },
            },
        };

    for (const auto& [point, jacobian] : points_and_jacobians) {
        const auto jac = geometry.from_onb_jacobian(point);
        TEST_EQUAL_RANGES(jac, jacobian);
    }
}

BOOST_FIXTURE_TEST_CASE(metric, CartesianEmbeddedEuclideanGeometry5DFixture,
                        *but::tolerance(epsilon)) {
    using namespace ray::tensor;

    const std::vector<std::pair<const ray::Vec<5_D>, const ray::Mat<5_D, 5_D>>>
        points_and_metrics = {
            {
                ray::Vec<5_D>{1.0, 2.0, 3.0, 4.0, 5.0},
                identity_mat<5_D>,
            },
        };

    for (const auto& [point, metric] : points_and_metrics) {
        const auto met = geometry.metric(point);
        TEST_EQUAL_RANGES(met, metric);
    }
}
