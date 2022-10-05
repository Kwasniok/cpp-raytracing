#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/diffuse.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace cpp_raytracing { namespace test {

TEST_CASE("diffuse") {
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
    const Vec3 direction_in = {1.0, 0.0, 0.0};
    for (int counter = 0; counter < 10; ++counter) {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        CHECK(ray_col == mat_col);
        CHECK_FALSE(direction_out.near_zero(Diffuse::epsilon));
        const Vec3 vec = direction_out - record.normal;
        CHECK_IN_RANGE(0.0, 1.0, vec.length());
    }
}

}} // namespace cpp_raytracing::test
