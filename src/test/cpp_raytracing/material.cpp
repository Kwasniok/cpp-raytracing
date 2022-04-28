#include <cmath>
#include <memory>

#include <cpp_raytracing/material.hpp>

#include "test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.0e-12;

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

     outcome: ray is probabilistic reflected or refracted based on the angle and
              IOR
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

     outcome: ray is probabilistic reflected or refracted based on the angle and
              IOR
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
    run(test_emitter);
    run(test_diffuse);
    run(test_metal_no_roughness);
    run(test_metal_with_roughness);
    run(test_dielectric_air);
    run(test_dielectric_into_glass);
    run(test_dielectric_total_reflection);
}

}} // namespace cpp_raytracing::test
