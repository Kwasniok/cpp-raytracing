#include "../../../common.hpp"

#include <memory>

#include <cpp_raytracing/world/materials/emitter.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace cpp_raytracing { namespace test {

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
        CHECK(ray_col == mat_col);
        CHECK(direction_out == Vec3(0.0, 0.0, 0.0));
    }
}

}} // namespace cpp_raytracing::test
