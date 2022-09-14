/**
 * @file
 * @brief basic example executable using triangles in curved space
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

constexpr std::array<Scalar, 2> sides = {-1.0, +1.0};
constexpr std::array<Scalar, 2> halfs = {-1.0, +1.0};
constexpr std::array<unsigned int, 3> axes = {0, 1, 2};

/** returns one triangle of the standard cube */
std::shared_ptr<Triangle> make_triangle(const Scalar side, const Scalar half,
                                        const unsigned int axis) {
    auto tri = std::make_shared<Triangle>();
    tri->id.change("triangle");
    tri->points = {
        Vec3{-1.0, -1.0, side},
        Vec3{half, -half, side},
        Vec3{+1.0, +1.0, side},
    };

    const auto perm0 = [](const Vec3 v) { return Vec3{v[0], v[1], v[2]}; };
    const auto perm1 = [](const Vec3 v) { return Vec3{v[1], v[2], v[0]}; };
    const auto perm2 = [](const Vec3 v) { return Vec3{v[2], v[0], v[1]}; };
    const std::array<std::function<Vec3(const Vec3)>, 3> perms = {
        perm0,
        perm1,
        perm2,
    };

    for (auto& p : tri->points) {
        p = perms[axis % 3](p);
    }

    // ensure outward facing surface normals
    if (half < 0.0) {
        std::swap(tri->points[0], tri->points[1]);
    }

    return tri;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    Scene scene;
    scene.active_camera = std::make_shared<Camera>(
        Camera::from({1.0, 1.5, 2.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 90.0,
                     16.0 / 9.0, 0.0));

    std::vector<std::shared_ptr<Material>> materials;

    // diffuse (gray)
    std::shared_ptr<Material> diffuse_gray;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("diffuse gray");
        auto texture = std::make_shared<Checker3D>();
        texture->color1 = {0.45, 0.45, 0.45};
        texture->color2 = {0.55, 0.55, 0.55};
        mat->color = std::move(texture);
        materials.push_back(mat);
        diffuse_gray = std::move(mat);
    }
    // diffuse (red)
    std::shared_ptr<Material> diffuse_red;
    {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("diffuse red");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {0.75, 0.5, 0.5};
        mat->color = std::move(texture);
        materials.push_back(mat);
        diffuse_red = std::move(mat);
    }
    // light
    std::shared_ptr<Material> light;
    {
        auto mat = std::make_shared<Emitter>();
        mat->id.change("light");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = 20.0 * Color{0.95, 0.9, 0.85};
        mat->color = std::move(texture);
        materials.push_back(mat);
        light = std::move(mat);
    }

    // cube
    for (auto side : sides) {
        for (auto half : halfs) {
            for (auto axis : axes) {
                auto tri = make_triangle(side, half, axis);
                tri->material = diffuse_red;
                scene.add(std::move(tri));
            }
        }
    }

    // floor (this)
    {
        constexpr Scalar L = 1e4;
        auto tri = std::make_shared<Triangle>();
        tri->id.change("floor");
        tri->points = {
            // note: upward surface normal
            Vec3{-L, -1.0, -L},
            Vec3{-L, -1.0, +L},
            Vec3{+L, -1.0, +L},
        };
        tri->material = diffuse_gray;
        scene.add(std::move(tri));
    }
    // floor (that)
    {
        constexpr Scalar L = 1e4;
        auto tri = std::make_shared<Triangle>();
        tri->id.change("floor");
        tri->points = {
            // note: upward surface normal
            Vec3{+L, -1.0, -L},
            Vec3{-L, -1.0, -L},
            Vec3{+L, -1.0, +L},
        };
        tri->material = diffuse_gray;
        scene.add(std::move(tri));
    }

    // top light
    {
        constexpr Scalar L = 1.0;
        constexpr Scalar H = 3.0;
        auto tri = std::make_shared<Triangle>();
        tri->id.change("light");
        tri->points = {
            Vec3{+L, H, -L},
            Vec3{-L, H, -L},
            Vec3{+L, H, +L},
        };
        tri->material = light;
        scene.add(std::move(tri));
    }

    return scene;
}

/** @brief configuration for render_ppm */
struct RenderConfig {
    /** @brief wheather to log detailed information during the render process */
    bool verbose;
    /** @brief path to output file (excluding extensions) */
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
    /** @brief gamma correction for non-raw images */
    ColorScalar gamma;
    /** @brief debug normals */
    bool debug_normals;
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

    EuclideanGeometry geometry;
    Scene scene = make_scene();

    GlobalShutterRenderer renderer;
    renderer.canvas = canvas;
    renderer.samples = config.samples;
    renderer.ray_depth = config.ray_depth;
    renderer.ray_color_if_ray_ended = {0.5, 0.7, 1.0}; // global illumination
    renderer.infrequent_callback_frequency = config.save_frequency;
    renderer.time = config.time;
    renderer.debug_normals = config.debug_normals;

    renderer.frequent_render_callback =
        [](const Renderer::State& current_state) {
            cout << "samples: " << current_state.samples << endl;
        };

    renderer.infrequent_render_callback =
        [&config](const Renderer::State& current_state) {
            cerr << "save current ..." << endl;
            write_image(config.path + ".current", current_state.image,
                        1.0 / ColorScalar(current_state.samples), config.gamma);
        };

    if (config.verbose) {
        cerr << "resolution factor = " << config.resolution_factor << endl;
        cerr << "cores detected = " << omp_get_num_procs() << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer.render(geometry, scene);
    write_image(config.path, image, 1.0, config.gamma);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    argparse::ArgumentParser parser("basic");
    parser.add_argument("-o", "--out")
        .required()
        .help("file output path (excluding extensions)");
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
    parser.add_argument("--gamma")
        .default_value<ColorScalar>(2.0)
        .help("gamma correction for non-raw image formats")
        .scan<'f', ColorScalar>();
    parser.add_argument("--debug_normals")
        .default_value<bool>(false) // store_true
        .implicit_value(true)
        .help("enable render mode to debug surface normals");

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
    config.gamma = parser.get<ColorScalar>("--gamma");
    config.debug_normals = parser.get<bool>("--debug_normals");

    render_ppm(config);
}
