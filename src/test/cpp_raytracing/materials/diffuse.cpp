#include <memory>

#include <cpp_raytracing/materials/diffuse.hpp>
#include <cpp_raytracing/textures/constant_color.hpp>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.0e-12;

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
    std::shared_ptr<Material> mat;
    {
        auto diffuse = std::make_unique<Diffuse>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        diffuse->color = std::move(texture);
        mat = std::move(diffuse);
    }
    const HitRecord record{
        .point = Vec3{1.0, 0.0, 0.0},
        .normal = Vec3{-1.0, 0.0, 0.0},
        .material = mat.get(),
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

void run_test_suite() {
    run(test_diffuse);
}

}} // namespace cpp_raytracing::test
