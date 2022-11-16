#define BOOST_TEST_MODULE cpp_raytracing::world::materials::metal

#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(metal_no_roughness, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length, ray::tensor::near_zero,
        ray::tensor::dot;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Scalar mat_rough = 0.0;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto metal = std::make_unique<ray::Metal3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        metal->color = std::move(texture);
        metal->roughness = mat_rough;
        mat = std::move(metal);
    }
    const ray::Vec3 normal{-1.0, 0.0, 0.0};
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .onb_normal = normal,
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = unit_vector(ray::Vec3{1.0, 1.0, 0.0});
    {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);
        // parallel to normal
        BOOST_CHECK(dot(normal, direction_out) == -dot(normal, direction_in));
        // orthogonal to normal
        const auto v1 = direction_out - normal * dot(normal, direction_out);
        const auto v2 = direction_in - normal * dot(normal, direction_in);
        TEST_EQUAL_RANGES(v1, v2);
    }
}

BOOST_AUTO_TEST_CASE(metal_with_roughness, *but::tolerance(epsilon)) {
    using ray::tensor::unit_vector, ray::tensor::length, ray::tensor::near_zero,
        ray::tensor::dot;

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
    const ray::Color mat_col{0.0, 0.5, 1.0};
    const ray::Scalar mat_rough = 0.25;
    std::shared_ptr<ray::Material3D> mat;
    {
        auto metal = std::make_unique<ray::Metal3D>();
        auto texture = std::make_shared<ray::ConstantColor3D>();
        texture->color = mat_col;
        metal->color = std::move(texture);
        metal->roughness = mat_rough;
        mat = std::move(metal);
    }
    const ray::Vec3 normal{-1.0, 0.0, 0.0};
    const ray::HitRecord3D record{
        .point = ray::Vec3{1.0, 0.0, 0.0},
        .onb_normal = normal,
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const ray::Vec3 direction_in = unit_vector(ray::Vec3{1.0, 1.0, 0.0});
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        TEST_EQUAL_RANGES(ray_col, mat_col);
        const ray::Vec3 in_para = normal * dot(normal, direction_in);
        const ray::Vec3 in_ortho = direction_in - in_para;
        // test if rand_vec in sphere of radius roughness
        const ray::Vec3 rand_vec = direction_out + in_para - in_ortho;
        CHECK_IN_BOUNDS(length(rand_vec), 0.0, mat_rough);
    }
}
