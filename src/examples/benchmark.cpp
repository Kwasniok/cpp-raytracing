/**
 * @file
 * @brief benchark executable
 * @note Benchmark scene.
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

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    const Camera camera = Camera::from({0.0, 1.0, 0.0}, {0.0, 0.0, -5.0},
                                       {0.0, 1.0, 0.0}, 90, 16.0 / 9.0, 0.02);

    Scene scene{camera};

    const int num_material_variations = 250;
    const int num_spheres = 2000;

    std::vector<std::shared_ptr<Material>> materials;
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Diffuse>();
        mat->id.change("diffuse");
        mat->color = Color::random(0.0, 1.0);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Metal>();
        mat->id.change("metal");
        mat->color = Color::random(0.6, 1.0);
        mat->roughness = random_scalar(0.0, 1.0);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Dielectric>();
        mat->id.change("glass");
        mat->color = Color::random(0.7, 1.0);
        mat->index_of_refraction = random_scalar(1.0, 2.5);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Emitter>();
        mat->id.change("light");
        mat->color = Color::random(0.7, 1.0);
        materials.emplace_back(std::move(mat));
    }

    // spheres
    for (int i = 0; i < num_spheres; ++i) {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->id.change("sphere");
        const auto x = random_scalar(-5.0, +5.0);
        const auto y = 0.1;
        const auto z = random_scalar(-100.0, -1.0);
        sphere->position = Vec3(x, y, z);
        sphere->radius = std::abs(y);
        sphere->material = materials[rand() % materials.size()];
        scene.add(std::move(sphere));
    }
    // floor
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(0.0, -100000.0, 0.0);
        sphere->radius = 100000.0;
        sphere->material = materials[rand() % materials.size()];
        scene.add(std::move(sphere));
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
    /** @brief depth per ray */
    unsigned long ray_depth;
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
    argparse::ArgumentParser parser("complex");
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
    parser.add_argument("--ray_depth")
        .required()
        .help("depth per ray")
        .scan<'d', unsigned long>();

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
    config.ray_depth = parser.get<unsigned long>("--ray_depth");

    render_ppm(config);
}
