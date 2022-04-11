#include <iostream>
#include <memory>

#include "camera.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "render.hpp"
#include "scene.hpp"
#include "sphere.hpp"

using namespace std;
using namespace ray;

void make_scene(Scene& hittables) {
    hittables.push_back(make_unique<Sphere>(Vec3(0.0, 0.0, -1.0), 0.5));
    hittables.push_back(make_unique<Sphere>(Vec3(0.0, +0.5, -1.5), 0.5));
    hittables.push_back(make_unique<Sphere>(Vec3(0.0, -0.5, -1.5), 0.5));
    hittables.push_back(make_unique<Sphere>(Vec3(+0.5, 0.0, -1.5), 0.5));
    hittables.push_back(make_unique<Sphere>(Vec3(-0.5, 0.0, -1.5), 0.5));
}

void print_example_ppm_file() {
    // see: https://en.wikipedia.org/wiki/Netpbm#File_formats

    const bool logging = false;
    const unsigned long max_color = 255;
    const unsigned long samples = 1;

    const Camera camera{.canvas_width = 1000,
                        .canvas_height = 1000,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.0, 0.0, 0.0},
                        .direction_y = {0.0, 1.0, 0.0},
                        .direction_z = {0.0, 0.0, -1.0}};

    Scene hittables;
    make_scene(hittables);

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
                const Scalar x = 2.0 * (Scalar(i) / camera.canvas_width - 0.5);
                const Scalar y = 2.0 * (Scalar(j) / camera.canvas_height - 0.5);

                Ray ray = camera.ray_for_coords(x, y);
                pixel_color += ray_color(hittables, ray);
            }
            cout << "  ";
            write_color_as_int_triple(cout, pixel_color, samples);
        }
        cout << endl;
    }
    cerr << "Done." << endl;
}

int main(int argc, char** argv) { print_example_ppm_file(); }
