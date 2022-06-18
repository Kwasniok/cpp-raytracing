#include <memory>

#include <cpp_raytracing/materials/emitter.hpp>
#include <cpp_raytracing/textures/constant_color.hpp>

#include "../test.hpp"

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
    std::shared_ptr<Material> mat;
    {
        auto emitter = std::make_unique_for_overwrite<Emitter>();
        auto texture = std::make_shared<ConstantColor>();
        texture->color = mat_col;
        emitter->color = std::move(texture);
        mat = std::move(emitter);
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
    {
        auto [ray_out, ray_col] = mat->scatter(record, ray_in);
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        TEST_ASSERT_EQUAL(ray_out.start(), Vec3(1.0, 0.0, 0.0));
        TEST_ASSERT_EQUAL(ray_out.direction(), Vec3(0.0, 0.0, 0.0));
    }
}

void run_test_suite() {
    run(test_emitter);
}

}} // namespace cpp_raytracing::test
