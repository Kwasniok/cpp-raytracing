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

void write_raw_image(const string& path, const RawImage& image) {
    ofstream file;
    file.open(path);
    if (file) {
        write_raw_image_ppm(file, image);
    } else {
        cerr << "Could not open file " << path << endl;
    }
    file.close();
}

void render_example_ppm(const string& path, const bool preview) {

    const bool logging = true;
    const unsigned long samples = preview ? 5 : 50;
    const unsigned long ray_depth = preview ? 20 : 50;
    const unsigned long resolution_factor = preview ? 1 : 8; // 8 <-> 1080p

    const Camera camera{.canvas_width = 240 * resolution_factor,
                        .canvas_height = 135 * resolution_factor,
                        .origin = {0.0, 0.0, 0.0},
                        .direction_x = {1.6, 0.0, 0.0},
                        .direction_y = {0.0, 0.9, 0.0},
                        .direction_z = {0.0, 0.0, -0.8},
                        .lens_radius = 0.01};

    Scene scene;
    make_scene(scene);

    if (logging) {
        cerr << "resolution factor = " << resolution_factor << endl;
    }
    RawImage image = render(camera, scene, samples, ray_depth, logging);
    write_raw_image(path, image);
}

int main(int argc, char** argv) {
    const string path = "out/out.ppm";
    render_example_ppm(path, true);  // fast preview
    render_example_ppm(path, false); // proper render
}
