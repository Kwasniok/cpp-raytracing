#include <cmath>
#include <map>
#include <memory>
#include <tuple>

#include <cpp_raytracing/material.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.0e-12;

using SphereCoords = std::tuple<Scalar, Scalar>;

constexpr Vec3 unit_vector_from_coords(const Scalar theta, const Scalar phi) {
    return {
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        std::cos(theta),
    };
}

constexpr SphereCoords coords_from_unit_vector(const Vec3 vec) {
    return {
        std::atan2(vec.y(), vec.x()),
        std::acos(vec.z()),
    };
}

using SphereBin = std::tuple<int, int>;

struct StatisticsConfiguration {
    int theta_bins = 3;
    int phi_bins = 3;
    Scalar theta_min = 0.0;
    Scalar theta_max = SCALAR_PI;
    Scalar phi_min = 0.0;
    Scalar phi_max = 2 * SCALAR_PI;
    int repititions = 10;
};

template <typename T>
struct BinnedSphereMap {

    const StatisticsConfiguration& config;
    std::vector<T> data;

    BinnedSphereMap(const StatisticsConfiguration& config,
                    const T& initial = T()) // Note: this fails!
        : config(config), data(config.theta_bins * config.phi_bins, initial) {}
    BinnedSphereMap(BinnedSphereMap&&) = default;
    BinnedSphereMap(const BinnedSphereMap&) = delete;

    T& operator[](const SphereBin bin) {
        auto [theta, phi] = bin;
        return data[theta * config.phi_bins + phi];
    }
};

struct RayStatistics {

    const StatisticsConfiguration& config;
    BinnedSphereMap<int> heatmap;
    BinnedSphereMap<Color> total_color;
    int zero_vector_heat = 0;
    Color zero_vector_total_color{};

    RayStatistics(const StatisticsConfiguration& config)
        : config(config), heatmap(config), total_color(config) {}
    RayStatistics(RayStatistics&&) = default;
    RayStatistics(const RayStatistics&) = delete;

    int theta_bin(const Scalar theta) const {
        int bin = ((Scalar(theta) - config.theta_min) * config.theta_bins /
                   (config.theta_max - config.theta_min));
        if (bin == config.theta_bins) {
            return config.theta_bins - 1;
        }
        return bin;
    }

    int phi_bin(const Scalar phi) const {
        int bin = ((Scalar(phi) - config.phi_min) * config.phi_bins /
                   (config.phi_max - config.phi_min));
        if (bin == config.phi_bins) {
            return config.phi_bins - 1;
        }
        return bin;
    }

    void register_ray(const Ray& ray, const Color& color) {
        if (ray.direction().near_zero(epsilon)) {
            zero_vector_heat += 1;
            zero_vector_total_color += color;
        } else {
            const auto direction = unit_vector(ray.direction());
            const auto [theta, phi] = coords_from_unit_vector(direction);
            const auto theta_i = theta_bin(theta);
            const auto phi_i = theta_bin(phi);
            heatmap[{theta_i, phi_i}] += 1;
            total_color[{theta_i, phi_i}] += color;
        }
    }

    Color avg_color(const SphereBin bin) {
        if (heatmap[bin] > 0) {
            return total_color[bin] / heatmap[bin];
        } else {
            return Colors::BLACK;
        }
    }

    Color zero_vector_avg_color() {
        if (zero_vector_heat > 0) {
            return zero_vector_total_color / zero_vector_heat;
        } else {
            return Colors::BLACK;
        }
    }
};

struct Statistics {
    const StatisticsConfiguration& config;
    std::shared_ptr<Material> material;
    BinnedSphereMap<RayStatistics> ray_stats;

    Statistics(const StatisticsConfiguration& config,
               std::shared_ptr<Material> material)
        : config(config),
          material(material),
          ray_stats(config, RayStatistics(config)) {}
    Statistics(Statistics&&) = default;
    Statistics(const Statistics&) = delete;

    RayStatistics collect_ray_statistics_for_hit(const Ray& ray_in,
                                                 const HitRecord& record) {

        RayStatistics stats{config};
        for (int i = 0; i < config.repititions; ++i) {
            auto [ray_out, color] = material->scatter(record, ray_in);
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.start(), record.point,
                                              epsilon);
            stats.register_ray(ray_out, color);
        }
        return stats;
    }

    void collect_ray_statistics() {

        for (Scalar theta_i = 0; theta_i < config.theta_bins; ++theta_i) {
            const Scalar theta =
                theta_i * (config.theta_max / config.theta_bins);

            for (Scalar phi_i = 0; phi_i < config.phi_bins; ++phi_i) {
                const Scalar phi = phi_i * (config.phi_max / config.phi_bins);

                // ray from unit sphere hits segment of x-y plane in origin
                const Vec3 hit_point = {0.0, 0.0, 0.0};
                const Ray ray_in{
                    unit_vector_from_coords(theta, phi),
                    -unit_vector_from_coords(theta, phi),
                };
                const HitRecord record{
                    .point = hit_point,
                    .normal = {0.0, 0.0, 1.0},
                    .material = material,
                    .t = 1.0,
                    .front_face = true,
                };
                ray_stats[{theta_i, phi_i}] =
                    collect_ray_statistics_for_hit(ray_in, record);
            }
        }
    }
};

void test_example() {
    const StatisticsConfiguration config{
        .theta_bins = 18,
        .phi_bins = 36,
        .theta_min = 0.0,
        .theta_max = SCALAR_PI,
        .phi_min = 0.0,
        .phi_max = 2 * SCALAR_PI,
        .repititions = 10,
    };

    Color color{0.0, 0.5, 1.0};
    std::shared_ptr<Material> material = std::make_shared<Emitter>(color);
    Statistics stats{config, material};
}

void test_emitter() {
    /*
     sketch of hit scenario:

     ========X=========== surface
             ^        |
             |        v
             |     normal
             |
         incoming ray

     outcome: ray is absorbed
              outgoing ray has no direction
     */
    const Color mat_col{0.0, 0.5, 1.0};
    std::shared_ptr<Material> mat = std::make_shared<Emitter>(mat_col);
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        Vec3{1.0, 0.0, 0.0},
    };
    {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_EQUAL(ray_out.start(), Vec3(1.0, 0.0, 0.0));
        TEST_ASSERT_EQUAL(ray_out.direction(), Vec3(0.0, 0.0, 0.0));
    }
}

void test_diffuse() {
    /*
     sketch of hit scenario:

     ==================== surface
            /^        |
           / |        v
          /  |     normal
         /   |
        v    incoming ray
     outgoing ray

     outcome: ray is diffusely reflected
              outgoing ray has direction = normal + random unit vector
              (if zero, then equal to normal)
     */
    const Color mat_col{0.0, 0.5, 1.0};
    std::shared_ptr<Material> mat = std::make_shared<Diffuse>(mat_col);
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        Vec3{1.0, 0.0, 0.0},
    };
    for (int counter = 0; counter < 10; ++counter) {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_EQUAL(ray_out.start(), Vec3(1.0, 0.0, 0.0));
        TEST_ASSERT_FALSE(ray_out.direction().near_zero(Diffuse::epsilon));
        const Vec3 vec = ray_out.direction() - record.normal;
        TEST_ASSERT_IN_RANGE(vec.length(), 0.0, 1.0);
    }
}

void test_metal_no_roughness() {
    /*
     sketch of hit scenario:

     ==================== surface
            /\        |
           /  \       v
          /    \     normal
         /      \
        v    incoming ray
     outgoing ray

     outcome: ray is reflected
              outgoing ray is reflected + roughness * random vector in unit
              sphere
     */
    const Color mat_col{0.0, 0.5, 1.0};
    const Scalar mat_rough = 0.0;
    std::shared_ptr<Material> mat = std::make_shared<Metal>(mat_col, mat_rough);
    const Vec3 normal{-1.0, 0.0, 0.0};
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = normal,
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 in_direction = unit_vector(Vec3{1.0, 1.0, 0.0});
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        in_direction,
    };
    {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        const Vec3 out_direction = ray_out.direction();
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_EQUAL(ray_out.start(), Vec3(1.0, 0.0, 0.0));
        // parallel to normal
        TEST_ASSERT_EQUAL(dot(normal, out_direction),
                          -dot(normal, in_direction));
        // orthogonal to normal
        TEST_ASSERT_EQUAL(out_direction - normal * dot(normal, out_direction),
                          in_direction - normal * dot(normal, in_direction));
    }
}

void test_metal_with_roughness() {
    /*
     sketch of hit scenario:

     ==================== surface
            /\        |
           /  \       v
          /    \     normal
         /      \
        v    incoming ray
     outgoing ray

     outcome: ray is reflected
              outgoing ray is reflected + roughness * random vector in unit
              sphere
     */
    const Color mat_col{0.0, 0.5, 1.0};
    const Scalar mat_rough = 0.25;
    std::shared_ptr<Material> mat = std::make_shared<Metal>(mat_col, mat_rough);
    const Vec3 normal{-1.0, 0.0, 0.0};
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = normal,
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 in_direction = unit_vector(Vec3{1.0, 1.0, 0.0});
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        in_direction,
    };
    for (int counter = 0; counter < 10; ++counter) {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        const Vec3 out_direction = ray_out.direction();
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_EQUAL(ray_out.start(), Vec3(1.0, 0.0, 0.0));
        const Vec3 in_para = normal * dot(normal, in_direction);
        const Vec3 in_ortho = in_direction - in_para;
        // test if rand_vec in sphere of radius roughness
        const Vec3 rand_vec = out_direction + in_para - in_ortho;
        TEST_ASSERT_IN_RANGE(rand_vec.length(), 0.0, mat_rough);
    }
}

void test_dielectric_air() {
    /*
     sketch of hit scenario:

     outgoing ray (refraction)
        ^
         \
          \     IOR = 1
           \
            \
     ==================== surface
             \        |
              \       v
               \     normal     IOR = 1
                \
             incoming ray
     outgoing ray (reflection)

     outcome: ray is probabilistic reflected or refracted based on the angle
     and IOR
     */
    const Color mat_col{0.0, 0.5, 1.0};
    const Scalar ior = 1.0;
    std::shared_ptr<Material> mat = std::make_shared<Dielectric>(mat_col, ior);
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction = unit_vector({1.0, 1.0, 0.0});
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        direction,
    };
    for (int counter = 0; counter < 10; ++counter) {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.start(), Vec3(1.0, 0.0, 0.0),
                                          epsilon);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.direction(), direction,
                                          epsilon);
    }
}

void test_dielectric_into_glass() {
    /*
     sketch of hit scenario:

     outgoing ray (refraction)
            ^
            |
            |      IOR = 1.5
            |
            |
     ==================== surface
            /\        |
           /  \       v
          /    \     normal     IOR = 1
         /      \
        v    incoming ray
     outgoing ray (reflection)

     outcome: ray is probabilistic reflected or refracted based on the angle
     and IOR
     */
    const Color mat_col{0.0, 0.5, 1.0};
    const Scalar ior = 1.5;
    std::shared_ptr<Material> mat = std::make_shared<Dielectric>(mat_col, ior);
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector({1.0, 1.0, 0.0});
    const Vec3 direction_reflection = unit_vector({-1.0, 1.0, 0.0});
    // Snell's law for 45°
    const Scalar refraction_angle = std::asin(1 / std::sqrt(2) / ior);
    const Vec3 direction_refraction = {
        std::cos(refraction_angle),
        std::sin(refraction_angle),
        0.0,
    };
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        direction_in,
    };
    // collect statistics
    int refractions = 0; // ideal: ~95%
    int reflections = 0; // ideal: ~5%
    int total = 1000000; // must be >= 1000
    for (int counter = 0; counter < total; ++counter) {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.start(), Vec3(1.0, 0.0, 0.0),
                                          epsilon);
        try { // refraction
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.direction(),
                                              direction_refraction, epsilon);
            refractions += 1;
        } catch (const AssertionFailedException& e1) {
            try { // reflection
                TEST_ASSERT_ALMOST_EQUAL_ITERABLE(
                    ray_out.direction(), direction_reflection, epsilon);
                reflections += 1;
            } catch (const AssertionFailedException& e2) {
                std::stringstream msg;
                msg << "= " << ray_out.direction()
                    << " is neither a refraction " << direction_refraction
                    << " nor a reflection " << direction_reflection
                    << " with precision of epsilon = " << epsilon;
                throw AssertionFailedException(
                    internal::message("ray_out.direction()", __FILE__, __LINE__,
                                      msg.str().c_str()));
            }
        }
    }
    // check statistics
    TEST_ASSERT_IN_RANGE(refractions, int(0.93 * total), int(0.97 * total));
    TEST_ASSERT_IN_RANGE(reflections, int(0.03 * total), int(0.06 * total));
}

void test_dielectric_total_reflection() {
    /*
     sketch of hit scenario:

           IOR = 1 (<-> 1/1.5)

     ==================== surface
            /\        |
           /  \       v
          /    \     normal     IOR = 1.5 (<-> 1)
         /      \
        v    incoming ray
     outgoing ray (reflection)

     outcome: ray is reflected based on the angle and IOR
     note: IOR < sqrt(2) = 1.41... would result in partial refraction
     */
    const Color mat_col{0.0, 0.5, 1.0};
    const Scalar ior = 1 / 1.5;
    std::shared_ptr<Material> mat = std::make_shared<Dielectric>(mat_col, ior);
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat,
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector({1.0, 1.0, 0.0});
    const Vec3 direction_reflection = unit_vector({-1.0, 1.0, 0.0});
    const Ray ray_in{
        Vec3{0.0, 0.0, 0.0},
        direction_in,
    };
    int total = 1000000; // must be >= 1000
    for (int counter = 0; counter < total; ++counter) {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.start(), Vec3(1.0, 0.0, 0.0),
                                          epsilon);
        // reflection only
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(ray_out.direction(),
                                          direction_reflection, epsilon);
    }
}

void run_test_suite() {
    run(test_example);
    // run(test_emitter);
    // run(test_diffuse);
    // run(test_metal_no_roughness);
    // run(test_metal_with_roughness);
    // run(test_dielectric_air);
    // run(test_dielectric_into_glass);
    // run(test_dielectric_total_reflection);
}
}} // namespace cpp_raytracing::test
