/**
 * @file
 * @brief basic example executable
 * @note Basic scene.
 */

#include <argparse/argparse.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <omp.h>
#include <string>

#include <cpp_raytracing.hpp>

using namespace std;
using namespace cpp_raytracing;

/** @brief generate a sphere instance */
std::unique_ptr<Instance>
make_sphere(const Scalar radius, const std::shared_ptr<Material>& material) {
    auto sphere = std::make_shared_for_overwrite<Sphere>();
    sphere->radius = radius;
    sphere->material = material;

    auto instance = std::make_unique_for_overwrite<Instance>();
    instance->entity = sphere;

    return instance;
}

/** @brief generate a plane instance */
std::unique_ptr<Instance> make_plane(const std::shared_ptr<Material>& material,
                                     const bool finite = true) {
    auto plane = std::make_shared_for_overwrite<Plane>();
    plane->material = material;
    plane->finite_neg_x = finite;
    plane->finite_pos_x = finite;
    plane->finite_neg_y = finite;
    plane->finite_pos_y = finite;

    auto instance = std::make_unique_for_overwrite<Instance>();
    instance->entity = plane;

    return instance;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    Scene scene;
    scene.active_camera = std::make_unique<Camera>(
        Camera::from({0.0, 0.0, 0.0}, {0.0, 0.0, -0.8}, {0.0, 1.0, 0.0},
                     // note: The field of view had a bug originally and this
                     //       number is for backwards-compatibility.
                     102.6803835, // = artan(tan(90 / 2) / 0.8) * 2
                     16.0 / 9.0, 0.02));

    // diffuse (gray)
    std::shared_ptr<Material> diffuse_gray;
    {
        auto mat = std::make_unique_for_overwrite<Diffuse>();
        mat->id.change("diffuse gray");
        mat->color = {0.5, 0.5, 0.5};
        diffuse_gray = std::move(mat);
    }
    // diffuse (red)
    std::shared_ptr<Material> diffuse_red;
    {
        auto mat = std::make_unique_for_overwrite<Diffuse>();
        mat->id.change("diffuse red");
        mat->color = {0.75, 0.5, 0.5};
        diffuse_red = std::move(mat);
    }
    // metal
    std::shared_ptr<Material> metal;
    {
        auto mat = std::make_unique_for_overwrite<Metal>();
        mat->id.change("metal");
        mat->color = {0.8, 0.7, 0.6};
        mat->roughness = 0.2;
        metal = std::move(mat);
    }
    // glass
    std::shared_ptr<Material> glass;
    {
        auto mat = std::make_unique_for_overwrite<Dielectric>();
        mat->id.change("glass");
        mat->color = {1.0, 1.0, 1.0};
        mat->index_of_refraction = 1.5;
        glass = std::move(mat);
    }
    // light
    std::shared_ptr<Material> light;
    {
        auto mat = std::make_unique_for_overwrite<Emitter>();
        mat->id.change("light");
        mat->color = {0.95, 0.9, 0.85};
        light = std::move(mat);
    }

    // left
    {
        auto sphere = make_sphere(0.5, diffuse_red);
        sphere->id.change("left sphere");
        sphere->position = Vec3(-1.0, 0.0, -0.75);
        scene.add(std::move(sphere));
    }
    // middle (outer)
    {
        auto sphere = make_sphere(0.5, glass);
        sphere->id.change("middle sphere outer surface");
        sphere->position = Vec3(0.0, 0.0, -1.0);
        scene.add(std::move(sphere));
    }
    // middle (inner)
    {
        auto sphere = make_sphere(-0.4, glass);
        sphere->id.change("middle sphere inner surface");
        sphere->position = Vec3(0.0, 0.0, -1.0);
        scene.add(std::move(sphere));
    }
    // right
    {
        auto sphere = make_sphere(0.5, metal);
        sphere->id.change("right sphere");
        sphere->position = Vec3(+1.0, 0.0, -0.75);
        scene.add(std::move(sphere));
    }
    // top
    {
        auto sphere = make_sphere(0.5, light);
        sphere->id.change("top sphere");
        sphere->position = Vec3(0.0, 1.0, -1.0);
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

    return scene;
}

/**
 * @brief write raw image to ppm file
 * @param path path to ppm file
 * @param image raw image to be written
 * @param scale (optinal) factor to multiply each channel's value with
 */
void write_raw_image(const string& path, const RawImage& image,
                     const Scalar scale = 1.0) {
    ofstream file;
    file.open(path);
    if (file) {
        write_raw_image_ppm(file, image, scale);
    } else {
        cerr << "Could not open file " << path << endl;
    }
    file.close();
}

/**
 * @brief called after each sample for the entire image
 */
void frequent_render_callback(const Renderer::State& current_state) {
    cout << "samples: " << current_state.samples << endl;
}

/**
 * @brief called regularly to save the progress
 */
void infrequent_render_callback(const Renderer::State& current_state) {
    cerr << "save current ..." << endl;
    write_raw_image("out/current.ppm", current_state.image,
                    1.0 / Scalar(current_state.samples));
}

/** @brief configuration for render_ppm */
struct RenderConfig {
    /** @brief wheather to log detailed information during the render process */
    bool verbose;
    /** @brief path to ppm output file */
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
    renderer.frequent_render_callback = frequent_render_callback;
    renderer.infrequent_render_callback = infrequent_render_callback;
    renderer.infrequent_callback_frequency = config.save_frequency;
    renderer.time = config.time;

    if (config.verbose) {
        cerr << "resolution factor = " << config.resolution_factor << endl;
        cerr << "cores detected = " << omp_get_num_procs() << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer.render(scene);
    write_raw_image(config.path, image);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    argparse::ArgumentParser parser("basic");
    parser.add_argument("-o", "--out").required().help("file output path");
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

    render_ppm(config);
}
