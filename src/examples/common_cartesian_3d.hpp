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
    mesh->faces = {
        Face3D{0, 1, 3}, Face3D{3, 2, 0}, // -x
        Face3D{5, 4, 6}, Face3D{6, 7, 5}, // +x
        Face3D{1, 0, 4}, Face3D{4, 5, 1}, // -y
        Face3D{2, 3, 7}, Face3D{7, 6, 2}, // +y
        Face3D{4, 0, 2}, Face3D{2, 6, 4}, // -z
        Face3D{1, 5, 7}, Face3D{7, 3, 1}, // +z
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
    mesh->faces = {Face3D{0, 1, 3}, Face3D{3, 2, 0}};

    return mesh;
}

} // namespace cpp_raytracing::examples

#endif
