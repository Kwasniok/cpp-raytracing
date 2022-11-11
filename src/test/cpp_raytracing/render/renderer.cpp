#define BOOST_TEST_MODULE cpp_raytracing::render::renderer

#include "../../common.hpp"

#include <array>
#include <sstream>

#include <cpp_raytracing/geometry/euclidean.hpp>
#include <cpp_raytracing/render/renderer.hpp>
#include <cpp_raytracing/world/backgrounds/constant.hpp>
#include <cpp_raytracing/world/entities/mesh.hpp>
#include <cpp_raytracing/world/materials/emitter.hpp>
#include <cpp_raytracing/world/materials/metal.hpp>
#include <cpp_raytracing/world/textures/constant_color.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::ColorScalar epsilon = 1.0e-12;

/**
 * @brief returns a riangle as mesh entity
 */
std::shared_ptr<ray::Mesh> make_triange_mesh(const ray::Vec3& point0,
                                             const ray::Vec3& point1,
                                             const ray::Vec3& point2) {
    auto mesh = std::make_shared<ray::Mesh>();
    mesh->id.change("triangle mesh");
    mesh->points = {point0, point1, point2};
    mesh->faces = {ray::Face{0, 1, 2}};

    return mesh;
}

/** @brief returns constant color texture */
std::shared_ptr<ray::Texture> make_color_texture(const ray::Color& color) {
    auto texture = std::make_shared<ray::ConstantColor>();
    texture->color = color;
    return texture;
}

/** @brief returns metal material */
std::shared_ptr<ray::Material>
make_metal_material(const ray::Color& color, const ray::ColorScalar roughness) {
    auto mat = std::make_shared<ray::Metal>();
    mat->id.change("metal");
    mat->color = make_color_texture(color);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns emitter material */
std::shared_ptr<ray::Material>
make_emitter_material(const ray::Color& color,
                      const ray::ColorScalar strength = 1.0) {
    auto mat = std::make_shared<ray::Emitter>();
    mat->id.change("emitter");
    mat->color = make_color_texture(color * strength);
    return mat;
}

BOOST_AUTO_TEST_CASE(ray_color_euclidean_metal_reflection_background, *but::tolerance(epsilon)) {
    // ray hits reflective surface and scatters into background

    ray::EuclideanGeometry geometry;

    // test scene
    auto dummy_camera =
        std::make_shared<ray::PinholeCamera>(ray::cartesian_pinhole_camera(
            {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, 90.0, 1.0));
    ray::Scene scene(dummy_camera);

    // background (global illumination)
    const ray::Color background_color = {0.2, 0.3, 0.0};
    {
        auto background = std::make_shared<ray::ConstantBackground>();
        background->color = background_color;
        scene.active_background = std::move(background);
    }

    // target = perfectly reflective x-y plane
    const ray::Color material_color = {0.5, 0.7, 0.0};
    {
        // in x-y plane
        const ray::Scalar L = 1.0;
        auto tri =
            make_triange_mesh(ray::Vec3{-L, -L, 0.0}, ray::Vec3{+L, -L, 0.0},
                              ray::Vec3{0.0, L, 0.0});
        // perfectly reflective metal surface
        tri->material = make_metal_material(material_color, 0.0);
        scene.add(std::move(tri));
    }

    const auto frozen_scene = scene.freeze_for_time(0.0);

    // ray with orthonormal inclination with target
    const ray::Vec3 ray_start = {0.0, 0.0, 1.0};
    const ray::Vec3 ray_direction = {0.0, 0.0, -1.0};
    auto ray = geometry.ray_from(ray_start, ray_direction);
    BOOST_REQUIRE(ray);

    {
        // renderer
        ray::GlobalShutterRenderer renderer;
        ray::Color color_out =
            renderer.ray_color(geometry, frozen_scene, ray.get(), 2);

        // expect reflection + hit background
        const ray::Color color_expected = material_color * background_color;
        TEST_EQUAL_RANGES(color_out, color_expected);
    }
}

BOOST_AUTO_TEST_CASE(ray_color_euclidean_metal_reflection_emitter, *but::tolerance(epsilon)) {
    // ray hits reflective surface and scatters into emitter

    ray::EuclideanGeometry geometry;

    // test scene
    auto dummy_camera =
        std::make_shared<ray::PinholeCamera>(ray::cartesian_pinhole_camera(
            {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, 90.0, 1.0));
    ray::Scene scene(dummy_camera);

    // background (no global illumination)
    {
        auto background = std::make_shared<ray::ConstantBackground>();
        background->color = ray::Colors::BLACK;
        scene.active_background = std::move(background);
    }

    // target 1 = perfectly reflective x-y plane (z = +1)
    const ray::Color material_color1 = {0.2, 0.3, 0.0};
    {
        // in x-y plane
        const ray::Scalar L = 1.0;
        auto tri =
            make_triange_mesh(ray::Vec3{-L, -L, +1.0}, ray::Vec3{+L, -L, +1.0},
                              ray::Vec3{0.0, L, +1.0});
        // perfectly reflective metal surface
        tri->material = make_metal_material(material_color1, 0.0);
        scene.add(std::move(tri));
    }

    // target 2 = emitter x-y plane (z = -1)
    const ray::Color material_color2 = {0.5, 0.7, 0.0};
    {
        // in x-y plane
        const ray::Scalar L = 1.0;
        auto tri =
            make_triange_mesh(ray::Vec3{-L, -L, -1.0}, ray::Vec3{+L, -L, -1.0},
                              ray::Vec3{0.0, L, -1.0});
        // perfectly reflective metal surface
        tri->material = make_emitter_material(material_color2);
        scene.add(std::move(tri));
    }

    const auto frozen_scene = scene.freeze_for_time(0.0);

    // ray with orthonormal inclination with target
    const ray::Vec3 ray_start = {0.0, 0.0, 0.0};
    const ray::Vec3 ray_direction = {0.0, 0.0, 1.0};
    auto ray = geometry.ray_from(ray_start, ray_direction);
    BOOST_REQUIRE(ray);

    {
        // renderer
        ray::GlobalShutterRenderer renderer;
        ray::Color color_out =
            renderer.ray_color(geometry, frozen_scene, ray.get(), 2);
        // expect reflection + hit emitter
        const ray::Color color_expected = material_color1 * material_color2;
        TEST_EQUAL_RANGES(color_out, color_expected);
    }
}
