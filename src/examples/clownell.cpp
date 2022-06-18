/**
 * @file
 * @brief simple example executable for textures based on the 'Cornell box'
 * @note Texture test.
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

/** @brief generate a plane instance */
std::shared_ptr<Instance>
make_plane(const std::shared_ptr<Material>& material) {
    auto plane = std::make_shared<Plane>();
    plane->material = material;

    auto instance = std::make_shared<Instance>();
    instance->entity = plane;

    return instance;
}
/** @brief generate a sphere instance */
std::shared_ptr<Instance>
make_sphere(const Scalar radius, const std::shared_ptr<Material>& material) {
    auto sphere = std::make_shared<Sphere>();
    sphere->radius = radius;
    sphere->material = material;

    auto instance = std::make_shared<Instance>();
    instance->entity = sphere;

    return instance;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    // scale
    constexpr Scalar L = 5.55;
    constexpr Scalar H = L / 2.0;
    constexpr Scalar Q = H / 2.0;
    constexpr Scalar D = L / 555;

    Scene scene;
    scene.active_camera = std::make_shared<Camera>(
        Camera::from({2.78, 2.78, -8.00}, {2.78, 2.78, 0.0}, {0.0, 1.0, 0.0},
                     40.0, 1.0, 0.02));
    auto sky = std::make_shared<SimpleSky>();
    sky->color1 = {0.0, 1.0, 0.0};
    sky->color2 = {1.0, 0.0, 1.0};
    scene.active_background = std::move(sky);

    // materials
    std::shared_ptr<Material> green;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("green");
        auto checker = std::make_shared<Checker2D>();
        checker->color1 = {.12, .45, .15};
        checker->color2 = {.73, .73, .73};
        mat->color = std::move(checker);
        green = std::move(mat);
    }
    std::shared_ptr<Material> red;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("red");
        auto checker = std::make_shared<Checker2D>();
        checker->color1 = {.65, .05, .05};
        checker->color2 = {.73, .73, .73};
        mat->color = std::move(checker);
        red = std::move(mat);
    }
    std::shared_ptr<Material> white;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("white");
        auto checker = std::make_shared<Checker2D>();
        checker->color1 = {.15, .15, .55};
        checker->color2 = {.73, .73, .73};
        mat->color = std::move(checker);
        white = std::move(mat);
    }
    std::shared_ptr<Material> light;
    {
        auto mat = std::make_shared<Emitter>();
        mat->id.change("light");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {6.0, 6.0, 6.0};
        mat->color = std::move(texture);
        light = std::move(mat);
    }
    std::shared_ptr<Material> isotropic_white;
    {
        auto mat = std::make_shared<Isotropic>();
        mat->id.change("isotropic white");
        auto checker = std::make_shared<Checker3D>();
        checker->color1 = {1.0, 1.0, 1.0};
        checker->color2 = {0.5, 0.5, 0.5};
        checker->scale = Q / 2.0;
        checker->offset = Vec3{1.0 * Q, Q / 2.0, Q / 2.0};
        mat->color = std::move(checker);
        isotropic_white = std::move(mat);
    }
    std::shared_ptr<Material> isotropic_black;
    {
        auto mat = std::make_shared<Isotropic>();
        mat->id.change("isotropic black");
        auto checker = std::make_shared<Checker3D>();
        checker->color1 = {0.0, 0.0, 0.0};
        checker->color2 = {0.5, 0.5, 0.5};
        checker->scale = Q / 2.0;
        checker->offset = Vec3{0.0 * Q, Q / 2.0, Q / 2.0};
        mat->color = std::move(checker);
        isotropic_black = std::move(mat);
    }

    // planes
    {
        auto plane = make_plane(green);
        plane->id.change("plane left");
        plane->scale = {H, H, H};
        plane->rotation = Vec3(0.0, -pi / 2.0, 0.0);
        plane->position = Vec3(L, H, H);
        scene.add(std::move(plane));
    }
    {
        auto plane = make_plane(red);
        plane->id.change("plane right");
        plane->scale = {H, H, H};
        plane->rotation = Vec3(0.0, pi / 2.0, 0.0);
        plane->position = Vec3(0, H, H);
        scene.add(std::move(plane));
    }
    {
        auto plane = make_plane(light);
        plane->id.change("plane light top");
        plane->scale = {Q, Q, Q};
        plane->rotation = Vec3(pi / 2.0, 0.0, 0.0);
        plane->position = Vec3(H, L - D, H);
        scene.add(std::move(plane));
    }
    {
        auto plane = make_plane(white);
        plane->id.change("plane top");
        plane->scale = {H, H, H};
        plane->rotation = Vec3(pi / 2.0, 0.0, 0.0);
        plane->position = Vec3(H, L, H);
        scene.add(std::move(plane));
    }
    {
        auto plane = make_plane(white);
        plane->id.change("plane bottom");
        plane->scale = {H, H, H};
        plane->rotation = Vec3(-pi / 2.0, 0.0, 0.0);
        plane->position = Vec3(H, 0, H);
        scene.add(std::move(plane));
    }
    {
        auto plane = make_plane(white);
        plane->id.change("plane back");
        plane->scale = {H, H, H};
        plane->rotation = Vec3(0.0, 0.0, 0.0);
        plane->position = Vec3(H, H, L);
        scene.add(std::move(plane));
    }

    // white mist
    {
        auto sphere = make_sphere(Q, nullptr);
        sphere->id.change("sphere mist white");
        sphere->position = Vec3(1.0 * Q, H, H);

        auto mist = std::make_shared<Mist>();
        mist->id.change("mist white");
        mist->boundary = std::move(sphere);
        mist->material = isotropic_white;
        mist->density = 1.0;

        scene.add(std::move(mist));
    }
    // black mist
    {
        auto sphere = make_sphere(Q, nullptr);
        sphere->id.change("sphere mist white");
        sphere->position = Vec3(3.0 * Q, H, H);

        auto mist = std::make_shared<Mist>();
        mist->id.change("mist black");
        mist->boundary = std::move(sphere);
        mist->material = isotropic_black;
        mist->density = 1.0;

        scene.add(std::move(mist));
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
        // 1:1 ratio
        .width = 200 * config.resolution_factor,
        .height = 200 * config.resolution_factor,
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
