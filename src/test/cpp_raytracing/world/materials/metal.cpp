#include <memory>

#include <cpp_raytracing/world/materials.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1.0e-12;

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
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        // parallel to normal
        TEST_ASSERT_EQUAL(dot(normal, direction_out),
                          -dot(normal, direction_in));
        // orthogonal to normal
        TEST_ASSERT_EQUAL(direction_out - normal * dot(normal, direction_out),
                          direction_in - normal * dot(normal, direction_in));
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
        TEST_ASSERT_EQUAL(ray_col, mat_col);
        const Vec3 in_para = normal * dot(normal, direction_in);
        const Vec3 in_ortho = direction_in - in_para;
        // test if rand_vec in sphere of radius roughness
        const Vec3 rand_vec = direction_out + in_para - in_ortho;
        TEST_ASSERT_IN_RANGE(rand_vec.length(), 0.0, mat_rough);
    }
}

void run_test_suite() {
    run(test_metal_no_roughness);
    run(test_metal_with_roughness);
}

}} // namespace cpp_raytracing::test
