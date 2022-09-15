#include <memory>

#include <cpp_raytracing/world/materials/dielectric.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.0e-12;

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
    std::shared_ptr<Material> mat;
    {
        auto dielectric = std::make_unique<Dielectric>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector({1.0, 1.0, 0.0});
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(direction_out, direction_in, epsilon);
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
    std::shared_ptr<Material> mat;
    {
        auto dielectric = std::make_unique<Dielectric>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
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
    // collect statistics
    int refractions = 0; // ideal: ~95%
    int reflections = 0; // ideal: ~5%
    int total = 1000000; // must be >= 1000
    for (int counter = 0; counter < total; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        try { // refraction
            TEST_ASSERT_ALMOST_EQUAL_ITERABLE(direction_out,
                                              direction_refraction, epsilon);
            refractions += 1;
        } catch (const AssertionFailedException& e1) {
            try { // reflection
                TEST_ASSERT_ALMOST_EQUAL_ITERABLE(
                    direction_out, direction_reflection, epsilon);
                reflections += 1;
            } catch (const AssertionFailedException& e2) {
                std::stringstream msg;
                msg << "= " << direction_out << " is neither a refraction "
                    << direction_refraction << " nor a reflection "
                    << direction_reflection
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
    std::shared_ptr<Material> mat;
    {
        auto dielectric = std::make_unique<Dielectric>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector({1.0, 1.0, 0.0});
    const Vec3 direction_reflection = unit_vector({-1.0, 1.0, 0.0});
    int total = 1000000; // must be >= 1000
    for (int counter = 0; counter < total; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        // reflection only
        TEST_ASSERT_ALMOST_EQUAL_ITERABLE(direction_out, direction_reflection,
                                          epsilon);
    }
}

void run_test_suite() {
    run(test_dielectric_air);
    run(test_dielectric_into_glass);
    run(test_dielectric_total_reflection);
}

}} // namespace cpp_raytracing::test
