/**
 * @file
 * @brief main
 */

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "camera.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "image.hpp"
#include "material.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "sphere.hpp"

using namespace std;
using namespace ray;

/**
 * @brief generate an example scene
 */
Scene make_scene(const unsigned long resolution_factor) {

    const Camera camera{.canvas_width = 240 * resolution_factor,
                        .canvas_height = 135 * resolution_factor,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.6, 0.0, 0.0},
                        .direction_y = {0.0, 0.9, 0.0},
                        .direction_z = {0.0, 0.0, -0.8},
                        .lens_radius = 0.01};

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
    scene.add(make_unique<Sphere>(Vec3(-1.0, 0.0, -0.75), 0.5, diffuse_red));
    // middle (outer)
    scene.add(make_unique<Sphere>(Vec3(0.0, 0.0, -1.0), +0.5, glass));
    // middle (inner)
    scene.add(make_unique<Sphere>(Vec3(0.0, 0.0, -1.0), -0.4, glass));
    // right
    scene.add(make_unique<Sphere>(Vec3(+1.0, 0.0, -0.75), 0.5, metal));
    // above
    scene.add(make_unique<Sphere>(Vec3(+0.0, 1.0, -1.0), 0.5, light));
    // floor
    scene.add(
        make_unique<Sphere>(Vec3(0.0, -100.5, -1.0), 100.0, diffuse_gray));

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
    cout << current_samples << endl;
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

    Scene scene = make_scene(resolution_factor);

    Renderer renderer{.samples = preview ? 5 : 50u,
                      .ray_depth = preview ? 20u : 50u,
                      .render_callback = render_callback};
    if (logging) {
        cerr << "resolution factor = " << resolution_factor << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer.render(scene);
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
