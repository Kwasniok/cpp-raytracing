#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "camera.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "sphere.hpp"

using namespace std;
using namespace ray;

Color constexpr ray_back_ground_color(const Ray& ray) {
    Vec3 direction = ray.direction();
    direction = unit_vector(direction);
    auto t = 0.5 * abs(direction.y()) + 0.25;
    Color color = (1 - t) * Colors::WHITE + t * Colors::BLUE;
    return color;
}

Color normal_color(const HitRecord& record) {
    return 0.5 * Color(record.normal.x() + 1, record.normal.y() + 1,
                       record.normal.z() + 1);
}

Color ray_color(const vector<unique_ptr<Hittable>>& hittables, const Ray& ray) {
    HitRecord closest_record = {.t = SCALAR_INF};

    for (const auto& hittable : hittables) {
        HitRecord record = hittable->hit_record(ray);
        if (record.t < closest_record.t) {
            closest_record = record;
        }
    }

    if (closest_record.t < SCALAR_INF) {
        return normal_color(closest_record);
    }
    return ray_back_ground_color(ray);
}

void make_scene(vector<unique_ptr<Hittable>>& hittables) {
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

    const Camera camera{.canvas_width = 1000,
                        .canvas_height = 1000,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.0, 0.0, 0.0},
                        .direction_y = {0.0, 1.0, 0.0},
                        .direction_z = {0.0, 0.0, -1.0}};

    vector<unique_ptr<Hittable>> hittables;
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
            const Scalar x = 2.0 * (Scalar(i) / camera.canvas_width - 0.5);
            const Scalar y = 2.0 * (Scalar(j) / camera.canvas_height - 0.5);

            Ray ray = camera.ray_for_coords(x, y);
            Color color = ray_color(hittables, ray);
            cout << "  ";
            write_color_as_integrals(cout, color);
        }
        cout << endl;
    }
    cerr << "Done." << endl;
}

int main(int argc, char** argv) { print_example_ppm_file(); }
