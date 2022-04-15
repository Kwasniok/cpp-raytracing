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
    std::shared_ptr<Material> material =
        make_shared<Material>(Material{.diffuse_color = {0.5, 0.5, 0.5}});
    scene.add(make_unique<Sphere>(Vec3(0.0, 0.0, -1.0), 0.5, material));
    scene.add(make_unique<Sphere>(Vec3(0.0, -100.5, -1.0), 100.0, material));
}

void print_example_ppm_file() {
    // see: https://en.wikipedia.org/wiki/Netpbm#File_formats

    const bool logging = false;
    const unsigned long max_color = 255;
    const unsigned long samples = 100;
    const unsigned long ray_depth = 50;

    const Camera camera{.canvas_width = 250,
                        .canvas_height = 250,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.0, 0.0, 0.0},
                        .direction_y = {0.0, 1.0, 0.0},
                        .direction_z = {0.0, 0.0, -1.0}};

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
                Scalar x = Scalar(i) + random_scalar(0.0, 1.0);
                Scalar y = Scalar(j) + random_scalar(0.0, 1.0);
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
