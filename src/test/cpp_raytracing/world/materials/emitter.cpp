#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/emitter.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace cpp_raytracing { namespace test {

const Scalar epsilon = 1.0e-12;

TEST_CASE("emitter") {
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
        auto emitter = std::make_unique<Emitter>();
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
    const Vec3 direction_in = {1.0, 0.0, 0.0};
    {
        auto [direction_out, ray_col] = mat->scatter(record, direction_in);
        for (auto i = 0; i < 3; ++i) {
            CHECK(ray_col[i] == doctest::Approx(mat_col[i]).epsilon(epsilon));
        }
        for (auto i = 0; i < 3; ++i) {
            CHECK(direction_out[i] == 0.0);
        }
    }
}

}} // namespace cpp_raytracing::test
