#include "../../../common.hpp"

#include <cpp_raytracing/world/materials/base.hpp>

namespace cpp_raytracing { namespace test {

class Dummy : public Material {
    std::pair<Vec3, Color> scatter([[maybe_unused]] const HitRecord& record,
                                   const Vec3& ray_direction) const override {
        return {ray_direction, Colors::WHITE};
    }
};

TEST_CASE("id") {
    Dummy dummy;
    dummy.id.change("test_id");
    CHECK(dummy.id.str() == "test_id");
}

}} // namespace cpp_raytracing::test
