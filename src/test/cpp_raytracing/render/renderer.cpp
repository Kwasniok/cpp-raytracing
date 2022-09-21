#include <array>
#include <sstream>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/render/renderer.hpp>
#include <cpp_raytracing/world/backgrounds/constant.hpp>
#include <cpp_raytracing/world/entities/mesh.hpp>
#include <cpp_raytracing/world/materials/emitter.hpp>
#include <cpp_raytracing/world/materials/metal.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

constexpr ColorScalar epsilon = 1e-12;

/**
 * @brief returns a riangle as mesh entity
 */
std::shared_ptr<Mesh> make_triange_mesh(const Vec3& point0, const Vec3& point1,
                                        const Vec3& point2) {
    auto mesh = std::make_shared<Mesh>();
    mesh->id.change("triangle mesh");
    mesh->points = {point0, point1, point2};
    mesh->faces = {Face{0, 1, 2}};

    return mesh;
}

/** @brief returns constant color texture */
std::shared_ptr<Texture> make_color_texture(const Color& color) {
    auto texture = std::make_shared<ConstantColor>();
    texture->color = color;
    return texture;
}

/** @brief returns metal material */
std::shared_ptr<Material> make_metal_material(const Color& color,
                                              const ColorScalar roughness) {
    auto mat = std::make_shared<Metal>();
    mat->id.change("metal");
    mat->color = make_color_texture(color);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns emitter material */
std::shared_ptr<Material>
make_emitter_material(const Color& color, const ColorScalar strength = 1.0) {
    auto mat = std::make_shared<Emitter>();
    mat->id.change("emitter");
    mat->color = make_color_texture(color * strength);
    return mat;
}

void test_ray_color_euclidean_metal_reflection_background() {

    // ray hits reflective surface and scatters into background

    // ******* SETUP ********

    // geometry
    EuclideanGeometry geometry;

    // test scene
    auto dummy_camera =
        std::make_shared<PinholeCamera>(cartesian_pinhole_camera(
            {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, 90.0, 1.0));
    Scene scene(dummy_camera);

    // background (global illumination)
    constexpr Color background_color = {0.2, 0.3, 0.0};
    {
        auto background = std::make_shared<ConstantBackground>();
        background->color = background_color;
        scene.active_background = std::move(background);
    }

    // target = perfectly reflective x-y plane
    constexpr Color material_color = {0.5, 0.7, 0.0};
    {
        // in x-y plane
        constexpr Scalar L = 1.0;
        auto tri = make_triange_mesh(Vec3{-L, -L, 0.0}, Vec3{+L, -L, 0.0},
                                     Vec3{0.0, L, 0.0});
        // perfectly reflective metal surface
        tri->material = make_metal_material(material_color, 0.0);
        scene.add(std::move(tri));
    }

    const auto frozen_scene = scene.freeze_for_time(0.0);

    // ray with orthonormal inclination with target
    constexpr Vec3 ray_start = {0.0, 0.0, 1.0};
    constexpr Vec3 ray_direction = {0.0, 0.0, -1.0};
    auto ray = geometry.ray_from(ray_start, ray_direction);

    // ******* TEST *******

    // renderer
    GlobalShutterRenderer renderer;
    Color color_out = renderer.ray_color(geometry, frozen_scene, ray.get(), 2);

    // expect reflection + hit background
    constexpr Color color_expected = material_color * background_color;
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(color_out, color_expected, epsilon);
}

void test_ray_color_euclidean_metal_reflection_emitter() {

    // ray hits reflective surface and scatters into emitter

    // ******* SETUP ********

    // geometry
    EuclideanGeometry geometry;

    // test scene
    auto dummy_camera =
        std::make_shared<PinholeCamera>(cartesian_pinhole_camera(
            {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, 90.0, 1.0));
    Scene scene(dummy_camera);

    // background (no global illumination)
    {
        auto background = std::make_shared<ConstantBackground>();
        background->color = Colors::BLACK;
        scene.active_background = std::move(background);
    }

    // target 1 = perfectly reflective x-y plane (z = +1)
    constexpr Color material_color1 = {0.2, 0.3, 0.0};
    {
        // in x-y plane
        constexpr Scalar L = 1.0;
        auto tri = make_triange_mesh(Vec3{-L, -L, +1.0}, Vec3{+L, -L, +1.0},
                                     Vec3{0.0, L, +1.0});
        // perfectly reflective metal surface
        tri->material = make_metal_material(material_color1, 0.0);
        scene.add(std::move(tri));
    }

    // target 2 = emitter x-y plane (z = -1)
    constexpr Color material_color2 = {0.5, 0.7, 0.0};
    {
        // in x-y plane
        constexpr Scalar L = 1.0;
        auto tri = make_triange_mesh(Vec3{-L, -L, -1.0}, Vec3{+L, -L, -1.0},
                                     Vec3{0.0, L, -1.0});
        // perfectly reflective metal surface
        tri->material = make_emitter_material(material_color2);
        scene.add(std::move(tri));
    }

    const auto frozen_scene = scene.freeze_for_time(0.0);

    // ray with orthonormal inclination with target
    constexpr Vec3 ray_start = {0.0, 0.0, 0.0};
    constexpr Vec3 ray_direction = {0.0, 0.0, 1.0};
    auto ray = geometry.ray_from(ray_start, ray_direction);

    // ******* TEST *******

    // renderer
    GlobalShutterRenderer renderer;
    Color color_out = renderer.ray_color(geometry, frozen_scene, ray.get(), 2);
    // expect reflection + hit emitter
    constexpr Color color_expected = material_color1 * material_color2;
    TEST_ASSERT_ALMOST_EQUAL_ITERABLE(color_out, color_expected, epsilon);
}

void run_test_suite() {
    run(test_ray_color_euclidean_metal_reflection_background);
    run(test_ray_color_euclidean_metal_reflection_emitter);
}

}} // namespace cpp_raytracing::test
