/**
 * @file
 * @brief main
 */

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

    const Camera camera = Camera::from({0.0, 0.0, 0.0}, {0.0, 0.0, 0.8},
                                       {0.0, 1.0, 0.0}, 90, 16.0 / 9.0, 0.02);

    Scene scene{camera};

    // diffuse
    std::shared_ptr<Material> diffuse_gray =
        make_shared<Diffuse>(Diffuse({0.5, 0.5, 0.5}));
    std::shared_ptr<Material> diffuse_red =
        make_shared<Diffuse>(Diffuse({0.75, 0.5, 0.5}));
    std::shared_ptr<Material> metal =
        make_shared<Metal>(Metal({0.8, 0.7, 0.6}, 0.2));
    std::shared_ptr<Material> glass =
        make_shared<Dielectric>(Dielectric({1.0, 1.0, 1.0}, 1.5));
    std::shared_ptr<Material> light =
        make_shared<Emitter>(Emitter({0.95, 0.9, 0.85}));

    // left
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(-1.0, 0.0, -0.75);
        sphere->radius = 0.5;
        sphere->material = diffuse_red;
        scene.add(std::move(sphere));
    }
    // middle (outer)
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(0.0, 0.0, -1.0);
        sphere->radius = 0.5;
        sphere->material = glass;
        scene.add(std::move(sphere));
    }
    // middle (inner)
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(0.0, 0.0, -1.0);
        sphere->radius = -0.4;
        sphere->material = glass;
        scene.add(std::move(sphere));
    }
    // right
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(+1.0, 0.0, -0.75);
        sphere->radius = 0.5;
        sphere->material = metal;
        scene.add(std::move(sphere));
    }
    // above
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(0.0, 1.0, -1.0);
        sphere->radius = 0.5;
        sphere->material = light;
        scene.add(std::move(sphere));
    }
    // floor
    {
        auto sphere = make_unique_for_overwrite<Sphere>();
        sphere->position = Vec3(0.0, -100.5, -1.0);
        sphere->radius = 100.0;
        sphere->material = diffuse_gray;
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

/**
 * @brief called after each sample for the entire image
 */
void render_callback(const RawImage& current_image,
                     const unsigned long current_samples) {
    cout << "samples: " << current_samples << endl;
    if (current_samples % 100 == 0) {
        write_raw_image("out/out_current.ppm", current_image,
                        1.0 / Scalar(current_samples));
    }
}

/**
 * @brief render and save example scene
 * @param path path to ppm file
 * @param preview generate a preview of reduced quality
 */
void render_example_ppm(const string& path, const bool preview) {

    const bool logging = true;
    // 2 <-> 480p, 8 <-> 1080p, 16 <-> 4k
    const unsigned long resolution_factor = preview ? 1 : 8;

    const Canvas canvas{
        // 16:9 ratio
        .width = 240 * resolution_factor,
        .height = 135 * resolution_factor,
    };

    Scene scene = make_scene();

    Renderer renderer{
        .samples = preview ? 5u : 50u,
        .ray_depth = preview ? 20u : 50u,
        .render_callback = render_callback,
    };

    if (logging) {
        cerr << "resolution factor = " << resolution_factor << endl;
        cerr << "cores detected = " << omp_get_num_procs() << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer.render(canvas, scene);
    write_raw_image(path, image);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    const string path = "out/out.ppm";
    render_example_ppm(path, true);  // fast preview
    render_example_ppm(path, false); // proper render
}
