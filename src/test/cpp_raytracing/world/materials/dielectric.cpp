#define BOOST_TEST_MODULE cpp_raytracing::world::materials::dielectric

#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/dielectric.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(dielectric_air, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Scalar ior = 1.0;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto dielectric = std::make_unique<ray::Dielectric3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .normal = ray::Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = unit_vector(ray::Vec3{1.0, 1.0, 0.0});
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);
        TEST_EQUAL_RANGES(direction_out, direction_in);
    }
}

BOOST_AUTO_TEST_CASE(dielectric_into_glass, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Scalar ior = 1.5;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto dielectric = std::make_unique<ray::Dielectric3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .normal = ray::Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = unit_vector(ray::Vec3{1.0, 1.0, 0.0});
    const ray::Vec3 direction_reflection =
        unit_vector(ray::Vec3{-1.0, 1.0, 0.0});
    // Snell's law for 45°
    const ray::Scalar refraction_angle = std::asin(1 / std::sqrt(2) / ior);
    const ray::Vec3 direction_refraction = {
        std::cos(refraction_angle),
        std::sin(refraction_angle),
        0.0,
    };
    // collect statistics
    int refractions = 0; // ideal: ~95%
    int reflections = 0; // ideal: ~5%
    int total = 100000;  // must be >= 1000 // NOLINT
    for (int counter = 0; counter < total; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);

        if (length(direction_out - direction_refraction) < epsilon) {
            refractions += 1;
        } else if (length(direction_out - direction_reflection) < epsilon) {
            reflections += 1;
        } else {
            std::stringstream msg;
            msg << "= " << direction_out << " is neither a refraction "
                << direction_refraction << " nor a reflection "
                << direction_reflection
                << " with precision of epsilon = " << epsilon;
            auto s = msg.str();
            BOOST_FAIL(s);
        }
    }
    // check statistics
    CHECK_IN_BOUNDS(refractions, int(0.93 * total), int(0.97 * total));
    CHECK_IN_BOUNDS(reflections, int(0.03 * total), int(0.06 * total));
}

BOOST_AUTO_TEST_CASE(dielectric_total_reflection, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Scalar ior = 1 / 1.5;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto dielectric = std::make_unique<ray::Dielectric3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        dielectric->color = std::move(texture);
        dielectric->index_of_refraction = ior;
        mat = std::move(dielectric);
    }
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .normal = ray::Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = unit_vector(ray::Vec3{1.0, 1.0, 0.0});
    const ray::Vec3 direction_reflection =
        unit_vector(ray::Vec3{-1.0, 1.0, 0.0});
    int total = 1000000; // must be >= 1000 // NOLINT
    for (int counter = 0; counter < total; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);
        // reflection only
        TEST_EQUAL_RANGES(direction_out, direction_reflection);
    }
}
