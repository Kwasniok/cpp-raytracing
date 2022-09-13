/**
 * @file
 * @brief example executable demonstrating portals
 * @note Portal demonstration scene.
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

/** @brief generate a sphere instance */
std::shared_ptr<Instance>
make_sphere(const Scalar radius, const std::shared_ptr<Material>& material) {
    auto sphere = std::make_shared<Sphere>();
    sphere->radius = radius;
    sphere->material = material;

    auto instance = std::make_shared<Instance>();
    instance->entity = sphere;

    return instance;
}

/** @brief generate a plane instance */
std::shared_ptr<Instance> make_plane(const std::shared_ptr<Material>& material,
                                     const bool finite = true) {
    auto plane = std::make_shared<Plane>();
    plane->material = material;
    plane->finite_neg_x = finite;
    plane->finite_pos_x = finite;
    plane->finite_neg_y = finite;
    plane->finite_pos_y = finite;

    auto instance = std::make_shared<Instance>();
    instance->entity = plane;

    return instance;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    Scene scene;
    scene.active_camera = std::make_shared<Camera>(
        Camera::from({0.0, 5.0, 10.0}, {0.0, 0.0, -0.25}, {0.0, 1.0, 0.0},
                     10.0, // major zoom-in for orthographic-like perspective
                     16.0 / 9.0, 0.02));

    // diffuse (gray)
    std::shared_ptr<Material> diffuse_gray;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("diffuse gray");
        auto texture = std::make_shared<Checker3D>();
        texture->color1 = {0.45, 0.45, 0.45};
        texture->color2 = {0.55, 0.55, 0.55};
        texture->scale = 0.125;
        mat->color = std::move(texture);
        diffuse_gray = std::move(mat);
    }
    // diffuse (red)
    std::shared_ptr<Material> diffuse_red;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("diffuse red");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {0.75, 0.5, 0.5};
        mat->color = std::move(texture);
        diffuse_red = std::move(mat);
    }
    // metal
    std::shared_ptr<Material> metal;
    {
        auto mat = std::make_shared<Metal>();
        mat->id.change("metal");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {0.8, 0.7, 0.6};
        mat->color = std::move(texture);
        mat->roughness = 0.2;
        metal = std::move(mat);
    }
    // glass
    std::shared_ptr<Material> glass;
    {
        auto mat = std::make_shared<Dielectric>();
        mat->id.change("glass");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {1.0, 1.0, 1.0};
        mat->color = std::move(texture);
        mat->index_of_refraction = 1.5;
        glass = std::move(mat);
    }
    // portal_a
    std::shared_ptr<Material> portal_a;
    {
        auto mat = std::make_shared<Portal>();
        mat->id.change("portal_a");
        mat->position = {+1.0, 0.0, 0.0};
        portal_a = std::move(mat);
    }
    // portal_b
    std::shared_ptr<Material> portal_b;
    {
        auto mat = std::make_shared<Portal>();
        mat->id.change("portal_b");
        mat->position = {-1.0, 0.0, 0.0};
        portal_b = std::move(mat);
    }
    // light
    std::shared_ptr<Material> light;
    {
        auto mat = std::make_shared<Emitter>();
        mat->id.change("light");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = 5.0 * Color{0.95, 0.9, 0.85};
        mat->color = std::move(texture);
        light = std::move(mat);
    }

    // middle_a
    {
        auto plane = make_plane(portal_a, true);
        plane->id.change("middle plane portal_a");
        plane->position = {-0.5, 0.0, -0.25};
        plane->rotation = {0.0, 0.0, pi / 8.0};
        plane->scale = {0.4, 0.4, 0.4};
        scene.add(std::move(plane));
    }
    // middle_b
    {
        auto plane = make_plane(portal_b, true);
        plane->id.change("middle plane portal_b");
        plane->position = {+0.5, 0.0, -0.25};
        plane->rotation = {0.0, 0.0, pi / 8.0};
        plane->scale = {0.4, 0.4, 0.4};
        scene.add(std::move(plane));
    }
    // background left
    {
        auto sphere = make_sphere(0.5, diffuse_red);
        sphere->id.change("background sphere left");
        sphere->position = Vec3(-1.0, 0.0, -1.0);
        scene.add(std::move(sphere));
    }
    // background right
    {
        auto sphere = make_sphere(0.5, metal);
        sphere->id.change("background sphere right");
        sphere->position = Vec3(+1.0, 0.0, -1.0);
        scene.add(std::move(sphere));
    }
    // background right light
    {
        auto sphere = make_sphere(0.25, light);
        sphere->id.change("background sphere right light");
        sphere->position = Vec3(+1.0, 1.5, -1.0);
        scene.add(std::move(sphere));
    }
    // background middle
    {
        auto sphere = make_sphere(0.5, diffuse_gray);
        sphere->id.change("background sphere middle");
        sphere->position = Vec3(0.0, 0.0, -1.0);
        scene.add(std::move(sphere));
    }
    // floor
    {
        auto plane = make_plane(diffuse_gray, false);
        plane->id.change("floor");
        plane->rotation = Vec3(pi / 2.0, 0.0, 0.0);
        plane->position = Vec3(0.0, -0.5, -1.0);
        scene.add(std::move(plane));
    }
    // proper background
    {
        auto plane = make_plane(diffuse_gray, false);
        plane->id.change("background plane");
        plane->position = Vec3(0.0, 0.0, -2.0);
        scene.add(std::move(plane));
    }

    return scene;
}

/** @brief configuration for render_ppm */
struct RenderConfig {
    /** @brief wheather to log detailed information during the render process */
    bool verbose;
    /** @brief path to output file (excluding extensions) */
    string path;
    /**
     * @brief factor to upscale the resolution
     * @note 1 <-> 240p, 8 <-> 1080p, 16 <-> 4k
     */
    unsigned long resolution_factor;
    /** @brief samples per pixel */
    unsigned long samples;
    /** @brief save progress every n samples */
    unsigned long save_frequency;
    /** @brief depth per ray */
    unsigned long ray_depth;
    /** @brief time of the frame */
    Scalar time;
    /** @brief gamma correction for non-raw images */
    ColorScalar gamma;
};

/**
 * @brief render and save example scene
 */
void render_ppm(const RenderConfig& config) {

    const Canvas canvas{
        // 16:9 ratio
        .width = 240 * config.resolution_factor,
        .height = 135 * config.resolution_factor,
    };

    Scene scene = make_scene();

    GlobalShutterRenderer renderer;
    renderer.canvas = canvas;
    renderer.samples = config.samples;
    renderer.ray_depth = config.ray_depth;
    renderer.ray_color_if_ray_ended = Colors::WHITE; // global illumination
    renderer.infrequent_callback_frequency = config.save_frequency;
    renderer.time = config.time;

    renderer.frequent_render_callback =
        [](const Renderer::State& current_state) {
            cout << "samples: " << current_state.samples << endl;
        };

    renderer.infrequent_render_callback =
        [&config](const Renderer::State& current_state) {
            cerr << "save current ..." << endl;
            write_image(config.path + ".current", current_state.image,
                        1.0 / ColorScalar(current_state.samples), config.gamma);
        };

    if (config.verbose) {
        cerr << "resolution factor = " << config.resolution_factor << endl;
        cerr << "cores detected = " << omp_get_num_procs() << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer.render(scene);
    write_image(config.path, image, 1.0, config.gamma);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    argparse::ArgumentParser parser("basic");
    parser.add_argument("-o", "--out")
        .required()
        .help("file output path (excluding extensions)");
    parser.add_argument("-v", "--verbose")
        .default_value<bool>(false) // store_true
        .implicit_value(true)
        .help("enable logging");
    parser.add_argument("--resolution_factor")
        .default_value<unsigned long>(1)
        .help("resolution factor")
        .scan<'d', unsigned long>();
    parser.add_argument("--samples")
        .required()
        .help("samples per pixel")
        .scan<'d', unsigned long>();
    parser.add_argument("--save_frequency")
        .default_value<unsigned long>(10)
        .help("save progress every n samples")
        .scan<'d', unsigned long>();
    parser.add_argument("--ray_depth")
        .required()
        .help("depth per ray")
        .scan<'d', unsigned long>();
    parser.add_argument("--time")
        .default_value<Scalar>(0.0)
        .help("time of the frame")
        .scan<'f', Scalar>();
    parser.add_argument("--gamma")
        .default_value<ColorScalar>(2.0)
        .help("gamma correction for non-raw image formats")
        .scan<'f', ColorScalar>();

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    RenderConfig config;
    config.verbose = parser.get<bool>("-v");
    config.path = parser.get("-o");
    config.resolution_factor = parser.get<unsigned long>("--resolution_factor");
    config.samples = parser.get<unsigned long>("--samples");
    config.save_frequency = parser.get<unsigned long>("--save_frequency");
    config.ray_depth = parser.get<unsigned long>("--ray_depth");
    config.time = parser.get<Scalar>("--time");
    config.gamma = parser.get<ColorScalar>("--gamma");

    render_ppm(config);
}
