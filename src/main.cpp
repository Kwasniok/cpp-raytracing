#include <iostream>
#include <memory>

#include "camera.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "random.hpp"
#include "ray.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "sphere.hpp"

using namespace std;
using namespace ray;

void make_scene(Scene& scene) {
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
}

void print_example_ppm_file() {
    // see: https://en.wikipedia.org/wiki/Netpbm#File_formats

    const bool logging = false;
    const unsigned long max_color = 255;
    const unsigned long samples = 100;
    const unsigned long ray_depth = 50;

    const Camera camera{.canvas_width = 500,
                        .canvas_height = 250,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.6, 0.0, 0.0},
                        .direction_y = {0.0, 0.9, 0.0},
                        .direction_z = {0.0, 0.0, -0.8},
                        .lens_radius = 0.01};

    Scene scene;
    make_scene(scene);

    // header
    cout << "P3 # ASCII RGB" << endl;
    cout << camera.canvas_width << " " << camera.canvas_height
         << " # width x height" << endl;
    cout << max_color << " # max color value per channel" << endl;

    // content (checker board)
    if (logging) {
        cerr << "Rendering image ..." << endl;
    }
    for (long j = camera.canvas_height - 1; j >= 0; --j) {
        if (logging) {
            cerr << "line " << (j + 1) << "/" << camera.canvas_height << endl;
        }
        for (long i = 0; i < camera.canvas_width; ++i) {
            Color pixel_color = Colors::BLACK;
            for (long s = 0; s < samples; ++s) {
                // random sub-pixel offset for antialiasing
                Scalar x = Scalar(i) + random_scalar<-0.5, +0.5>();
                Scalar y = Scalar(j) + random_scalar<-0.5, +0.5>();
                // transform to camera coordinates
                x = (2.0 * x / camera.canvas_width - 1.0);
                y = (2.0 * y / camera.canvas_height - 1.0);

                Ray ray = camera.ray_for_coords(x, y);
                pixel_color += ray_color(scene, ray, ray_depth);
            }
            cout << "  ";
            write_color_as_int_triple(cout, pixel_color, samples);
        }
        cout << endl;
    }
    cerr << "Done." << endl;
}

int main(int argc, char** argv) { print_example_ppm_file(); }
