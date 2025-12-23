/**
 * @file
 * @brief common header for all 3D Cartesian examples
 */

#ifndef CPP_RAYTRACING_EXAMPLES_COMMON_CARTESIAN_3D_HPP
#define CPP_RAYTRACING_EXAMPLES_COMMON_CARTESIAN_3D_HPP

#include "common.hpp"

namespace cpp_raytracing::examples {

/**
 * @brief returns a 3D cube as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh3D> make_cube_3d(const Scalar scale, const Vec3& position) {
    using namespace tensor;

    auto mesh = std::make_shared<SmallTriangleMesh3D>();
    mesh->id.change("cube");
    mesh->points = {
        scale * Vec3{-1.0, -1.0, -1.0} + position, // 0
        scale * Vec3{-1.0, -1.0, +1.0} + position, // 1
        scale * Vec3{-1.0, +1.0, -1.0} + position, // 2
        scale * Vec3{-1.0, +1.0, +1.0} + position, // 3
        scale * Vec3{+1.0, -1.0, -1.0} + position, // 4
        scale * Vec3{+1.0, -1.0, +1.0} + position, // 5
        scale * Vec3{+1.0, +1.0, -1.0} + position, // 6
        scale * Vec3{+1.0, +1.0, +1.0} + position, // 7
    };
    const Mat2x2 m = make_diag_mat<2_D>({1, 1});
    const Vec2 v0 = zero_vec<2_D>;
    const Vec2 v1 = Vec2{1.0, 1.0};
    mesh->faces = {
        Face{.indices = {1, 3, 0}, .uv_map = {m, v0}},  // -x
        Face{.indices = {2, 0, 3}, .uv_map = {-m, v1}}, // -x
        Face{.indices = {4, 6, 5}, .uv_map = {m, v0}},  // +x
        Face{.indices = {7, 5, 6}, .uv_map = {-m, v1}}, // +x
        Face{.indices = {0, 4, 1}, .uv_map = {m, v0}},  // -y
        Face{.indices = {5, 1, 4}, .uv_map = {-m, v1}}, // -y
        Face{.indices = {3, 7, 2}, .uv_map = {m, v0}},  // +y
        Face{.indices = {6, 2, 7}, .uv_map = {-m, v1}}, // +y
        Face{.indices = {0, 2, 4}, .uv_map = {m, v0}},  // -z
        Face{.indices = {6, 4, 2}, .uv_map = {-m, v1}}, // -z
        Face{.indices = {5, 7, 1}, .uv_map = {m, v0}},  // +z
        Face{.indices = {3, 1, 7}, .uv_map = {-m, v1}}, // +z
    };

    return mesh;
}

/**
 * @brief returns a bounded x-z plane as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh3D> make_xz_plane(const Scalar scale,
                                      const Vec3& position) {
    auto mesh = std::make_shared<SmallTriangleMesh3D>();
    mesh->id.change("plane");
    mesh->points = {
        scale * Vec3{-1.0, 0.0, -1.0} + position, // 0
        scale * Vec3{-1.0, 0.0, +1.0} + position, // 1
        scale * Vec3{+1.0, 0.0, -1.0} + position, // 2
        scale * Vec3{+1.0, 0.0, +1.0} + position, // 3
    };
    mesh->faces = {Face{0, 1, 3}, Face{3, 2, 0}};

    return mesh;
}

} // namespace cpp_raytracing::examples

#endif
