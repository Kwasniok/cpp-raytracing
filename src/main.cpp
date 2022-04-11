#include <cmath>
#include <iostream>

#include "camera.hpp"
#include "color.hpp"
#include "ray.hpp"
#include "sphere.hpp"

using namespace std;
using namespace ray;

Scalar constexpr hits_at(const Ray& ray, const Sphere& sphere) {
    // returns minimal solution which is positive and starts outside of the
    // sphere or else nan
    constexpr Scalar nan = numeric_limits<Scalar>::signaling_NaN();

    // analytical geometry: line hits sphere
    // ray: s + t*d
    // sphere: (x-o)^2 = R^2
    // solve: a*t^2 + b*t + c = 0
    // where a = d^2 >= 0, b = 2*d*(s-o), c = (s-o)^2 - R^2
    // solution: t = (-b +/- sqrt(b^2 - 4ac))/(2a)
    const auto delta = ray.start() - sphere.origin();
    const auto a = dot(ray.direction(), ray.direction());
    const auto b = 2.0 * dot(ray.direction(), delta);
    const auto c = dot(delta, delta) - sphere.radius() * sphere.radius();
    const auto discriminant = b * b - 4.0 * a * c;
    if (discriminant < 0.0) {
        // no real solution
        return nan;
    }
    const auto t1 = (-b - sqrt(discriminant)) / (2.0 * a);
    // const auto t2 = (-b + sqrt(discriminant)) / (2.0 * a);
    // t1>0, t2>0: ray starts outside of the sphere and enters it at t1
    // t1<0, t2>0: ray starts inside the sphere and leaves it at t2
    // t1<0, t2<0: ray starts after the sphere and never enters it

    // select minimal positive sloution (if it exists)
    if (t1 > 0.0) {
        return t1;
    }
    // no positive soltion which is minimal
    return nan;
}

Color constexpr ray_back_ground_color(const Ray& ray) {
    Vec3 direction = ray.direction();
    direction = unit_vector(direction);
    auto t = 0.5 * abs(direction.y()) + 0.25;
    Color color = (1 - t) * Colors::WHITE + t * Colors::BLUE;
    return color;
}

Color constexpr ray_color(const Ray& ray) {
    constexpr Sphere sphere{{0.0, 0.0, -1.0}, 0.5};
    const auto t = hits_at(ray, sphere);
    if (t > 0.0) {
        const auto hit_point = ray.at(t);
        const auto normal = unit_vector(hit_point - sphere.origin());
        return 0.5 * Color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }
    return ray_back_ground_color(ray);
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
            Color color = ray_color(ray);
            cout << "  ";
            write_color_as_integrals(cout, color);
        }
        cout << endl;
    }
    cerr << "Done." << endl;
}

int main(int argc, char** argv) { print_example_ppm_file(); }
