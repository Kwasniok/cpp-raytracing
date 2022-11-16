#define BOOST_TEST_MODULE cpp_raytracing::world::materials::diffuse

#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/diffuse.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(diffuse, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length, ray::tensor::near_zero;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    std::shared_ptr<ray::Material3D> mat;
    {
        auto diffuse = std::make_unique<ray::Diffuse3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        diffuse->color = std::move(texture);
        mat = std::move(diffuse);
    }
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .normal = ray::Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = {1.0, 0.0, 0.0};
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);
        BOOST_CHECK(!near_zero(direction_out, ray::Diffuse3D::epsilon));
        const ray::Vec3 vec = direction_out - record.normal;
        CHECK_IN_BOUNDS(length(vec), 0.0, 1.0);
    }
}
