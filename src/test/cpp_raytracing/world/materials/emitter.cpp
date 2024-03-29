#define BOOST_TEST_MODULE cpp_raytracing::world::materials::emitter

#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/emitter.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

using ray::operator"" _D;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(emitter, *but::tolerance(epsilon)) {
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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Vec3 v0 = ray::tensor::zero_vec<3_D>;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto emitter = std::make_unique<ray::Emitter3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        emitter->color = std::move(texture);
        mat = std::move(emitter);
    }
    const ray::Vec3 direction_in = {1.0, 0.0, 0.0};
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .onb_normal = ray::Vec3{-1.0, 0.0, 0.0},
        .onb_ray_direction = direction_in,
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    {
        auto [direction_out, ray_col] = mat->scatter(record);

        TEST_EQUAL_RANGES(ray_col, mat_col);
        TEST_EQUAL_RANGES(direction_out, v0);
    }
}
