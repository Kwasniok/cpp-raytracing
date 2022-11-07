#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace cpp_raytracing { namespace test {

using namespace tensor;

const Scalar epsilon = 1.0e-12;

TEST_CASE("metal_no_roughness") {
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
    std::shared_ptr<Material> mat;
    {
        auto metal = std::make_unique<Metal>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        metal->color = std::move(texture);
        metal->roughness = mat_rough;
        mat = std::move(metal);
    }
    const Vec3 normal{-1.0, 0.0, 0.0};
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = normal,
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector(Vec3{1.0, 1.0, 0.0});
    {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        for (auto i = 0; i < 3; ++i) {
            CHECK(ray_col[i] == doctest::Approx(mat_col[i]).epsilon(epsilon));
        }
        // parallel to normal
        CHECK(dot(normal, direction_out) ==
              doctest::Approx(-dot(normal, direction_in)).epsilon(epsilon));
        // orthogonal to normal
        const auto v1 = direction_out - normal * dot(normal, direction_out);
        const auto v2 = direction_in - normal * dot(normal, direction_in);
        for (auto i = 0; i < 3; ++i) {
            CHECK(v1[i] == doctest::Approx(v2[i]).epsilon(epsilon));
        }
    }
}

TEST_CASE("metal_with_roughness") {
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
    std::shared_ptr<Material> mat;
    {
        auto metal = std::make_unique<Metal>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        metal->color = std::move(texture);
        metal->roughness = mat_rough;
        mat = std::move(metal);
    }
    const Vec3 normal{-1.0, 0.0, 0.0};
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = normal,
        .material = mat.get(),
        .t = 1.0,
        .front_face = true,
    };
    const Vec3 direction_in = unit_vector(Vec3{1.0, 1.0, 0.0});
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        for (auto i = 0; i < 3; ++i) {
            CHECK(ray_col[i] == doctest::Approx(mat_col[i]).epsilon(epsilon));
        }
        const Vec3 in_para = normal * dot(normal, direction_in);
        const Vec3 in_ortho = direction_in - in_para;
        // test if rand_vec in sphere of radius roughness
        const Vec3 rand_vec = direction_out + in_para - in_ortho;
        CHECK_IN_RANGE(0.0, mat_rough, length(rand_vec));
    }
}

}} // namespace cpp_raytracing::test
