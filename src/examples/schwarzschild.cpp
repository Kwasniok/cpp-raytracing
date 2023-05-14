/**
 * @file
 * @brief example executable of a black hole in Schwarzschild coordinates
 */

#include <algorithm>
#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

#include <cpp_raytracing/geometries/schwarzschild.hpp>

#include <cpp_raytracing/geometries/cartesian_embedded/sphere.hpp>

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;
namespace schwarzschild = cpp_raytracing::schwarzschild;

/** @brief make a 4-dimensional sphere */
std::shared_ptr<cartesian_embedded::Sphere<4_D>>
make_4d_sphere(const Scalar radius, const Vec<4_D> position = {}) {
    auto sphere = std::make_shared<cartesian_embedded::Sphere<4_D>>();
    sphere->radius = radius;
    sphere->position = position;

    return sphere;
}

/**
 * @brief generate a scene
 */
Scene<4_D> make_scene() {

    const Vec3 position{0.0, 0.0, 3.0};
    const Vec3 focus{0.0, 0.0, -3.0};
    const Vec3 up_direction{0.0, 1.0, 0.0};
    const Scalar field_of_view = 90.0; // deg
    const Scalar aspect_ratio = 16.0 / 9.0;

    auto camera = std::make_shared<schwarzschild::PerspectiveCamera>(
        schwarzschild::make_far_perspective_camera(
            position, focus, up_direction, field_of_view, aspect_ratio));

    Scene<4_D> scene(camera);

    // background (global illumination)
    {
        auto background = std::make_shared<ConstantBackground<4_D>>();
        background->color = {0.5, 0.7, 1.0};
        scene.active_background = std::move(background);
    }

    // materials
    auto diffuse_gray = make_diffuse_volume_checker_material<4_D>(
        Color{0.45, 0.45, 0.45}, Color{0.55, 0.55, 0.55});
    diffuse_gray->id.change("diffuse gray");

    auto diffuse_red = make_diffuse_material<4_D>(Color{0.75, 0.5, 0.5});
    diffuse_red->id.change("diffuse red");

    auto metal_gray = make_metal_volume_checker_material<4_D>(
        Color{0.45, 0.45, 0.45}, Color{0.55, 0.55, 0.55});
    metal_gray->id.change("metal gray");

    auto light = make_light_material<4_D>(Color{0.85, 0.95, 0.75}, 2.0);
    light->id.change("light");

    {
        auto sphere = make_4d_sphere(0.5, {-1.0, -1.0, -2.0});
        sphere->material = diffuse_red;
        scene.add(std::move(sphere));
    }

    {
        auto sphere = make_4d_sphere(0.5, {1.0, 1.0, -2.0});
        sphere->material = light;
        scene.add(std::move(sphere));
    }

    {
        auto sphere = make_4d_sphere(10.0, {0.0, 0.0, 0.0});
        sphere->material = diffuse_gray;
        scene.add(std::move(sphere));
    }

    return scene;
}

/** @brief configuration for render_ppm */
struct RenderConfig {
    /** @brief wheather to log detailed information during the render
     * process
     */
    bool verbose = false;
    /** @brief path to output file (excluding extensions) */
    string path;
    /**
     * @brief factor to upscale the resolution
     * @note 1 <-> 240p, 8 <-> 1080p, 16 <-> 4k
     */
    unsigned long resolution_factor = 1;
    /** @brief samples per pixel */
    unsigned long samples = 1;
    /** @brief save progress every n samples */
    unsigned long save_frequency = 1;
    /** @brief depth per ray */
    unsigned long ray_depth = 0;
    /** @brief time of the frame */
    Scalar time = 0.0;
    /** @brief shutter mode */
    std::string shutter_mode = SHUTTER_MODE_GLOBAL;
    /** @brief exposure frame */
    Scalar exposure_time = 0.0;
    /** @brief exposure line */
    Scalar total_line_exposure_time = 0.0;
    /** @brief gamma correction for non-raw images */
    ColorScalar gamma = 2.0;
    /** @brief debug normals */
    bool debug_normals = false;
    /** @brief debug premature ray termination */
    bool debug_ray_terminations = false;
    /** @brief speed of light */
    Scalar speed_of_light = 1.0;
    /** @brief size of black hole */
    Scalar schwarzschild_radius = 1.0;
    /** @brief initial ray segment length parameter */
    Scalar ray_initial_step_size = 1.0;
    /** @brief abs ray integration error */
    Scalar ray_error_abs = 1.0;
    /** @brief rel ray integration error */
    Scalar ray_error_rel = 1.0;
    /** @brief ray length limit */
    Scalar ray_max_length = 1.0;
    /**
     * @brief factor by which to stretch each ray segment
     * @note Should be a bit larger than `1.0` to avoid small scale geometrical
     *        banding.
     */
    Scalar ray_segment_length_factor = 1.0;
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

    schwarzschild::Geometry geometry{
        config.speed_of_light,
        config.schwarzschild_radius,
        config.ray_initial_step_size,
        config.ray_error_abs,
        config.ray_error_rel,
        config.ray_max_length,
        config.ray_segment_length_factor,
    };
    Scene<4_D> scene = make_scene();

    std::unique_ptr<Renderer<4_D>> renderer;

    if (config.shutter_mode == SHUTTER_MODE_GLOBAL) {
        auto rendr = std::make_unique<GlobalShutterRenderer<4_D>>();
        rendr->exposure_time = config.exposure_time;
        renderer = std::move(rendr);
    }
    if (config.shutter_mode == SHUTTER_MODE_ROLLING) {
        auto rendr = std::make_unique<RollingShutterRenderer<4_D>>();
        rendr->frame_exposure_time = config.exposure_time;
        rendr->total_line_exposure_time = config.total_line_exposure_time;
        renderer = std::move(rendr);
    }

    renderer->canvas = canvas;
    renderer->samples = config.samples;
    renderer->ray_depth = config.ray_depth;
    renderer->infrequent_callback_frequency = config.save_frequency;
    renderer->time = config.time;
    renderer->debug_normals = config.debug_normals;
    renderer->ray_color_if_ray_ended = config.debug_ray_terminations
                                           ? Color{0.0, 100.0, 0.0}
                                           : Color{0.0, 0.0, 0.0};

    renderer->frequent_render_callback =
        [](const typename Renderer<4_D>::State& current_state) {
            cout << "samples: " << current_state.samples << endl;
        };

    renderer->infrequent_render_callback =
        [&config](const typename Renderer<4_D>::State& current_state) {
            cerr << "save current ..." << endl;
            write_image(config.path + ".current", current_state.image,
                        1.0 / ColorScalar(current_state.samples), config.gamma);
        };

    if (config.verbose) {
        cerr << "resolution factor = " << config.resolution_factor << endl;
        cerr << "cores detected = " << omp_get_num_procs() << endl;
        cerr << "rendering image ... " << endl;
    }
    RawImage image = renderer->render(geometry, scene);
    write_image(config.path, image, 1.0, config.gamma);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    argparse::ArgumentParser parser;
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
        .default_value<unsigned long>(5)
        .help("samples per pixel")
        .scan<'d', unsigned long>();
    parser.add_argument("--save_frequency")
        .default_value<unsigned long>(10)
        .help("save progress every n samples")
        .scan<'d', unsigned long>();
    parser.add_argument("--ray_depth")
        .default_value<unsigned long>(75) // NOLINT
        .help("depth per ray")
        .scan<'d', unsigned long>();
    parser.add_argument("--time")
        .default_value<Scalar>(0.0)
        .help("time of the frame")
        .scan<'g', Scalar>();
    parser.add_argument("--shutter_mode")
        .default_value(SHUTTER_MODE_GLOBAL)
        .action([](const std::string& value) {
            if (std::find(SHUTTER_MODES.begin(), SHUTTER_MODES.end(), value) !=
                SHUTTER_MODES.end()) {
                return value;
            }
            throw std::runtime_error("Unknown shutter mode.");
        })
        .help("shutter mode for image exposure");
    parser.add_argument("--exposure_time")
        .default_value<Scalar>(0.0)
        .help("total exposure time per frame")
        .scan<'g', Scalar>();
    parser.add_argument("--total_line_exposure_time")
        .default_value<Scalar>(0.0)
        .help("exposure time per line (rolling shutter mode only)")
        .scan<'g', Scalar>();
    parser.add_argument("--gamma")
        .default_value<ColorScalar>(2.0)
        .help("gamma correction for non-raw image formats")
        .scan<'g', ColorScalar>();
    parser.add_argument("--debug_normals")
        .default_value<bool>(false) // store_true
        .implicit_value(true)
        .help("enable render mode to debug surface normals");
    parser.add_argument("--debug_ray_terminations")
        .default_value<bool>(false) // store_true
        .implicit_value(true);
    parser.add_argument("--speed_of_light")
        .default_value<Scalar>(100.0)
        .help("speed of light")
        .scan<'g', Scalar>();
    parser.add_argument("--schwarzschild_radius")
        .default_value<Scalar>(1.0)
        .help("size of black hole")
        .scan<'g', Scalar>();
    parser.add_argument("--ray_initial_step_size")
        .default_value<Scalar>(1e-1)
        .help("influences initial length of ray segments")
        .scan<'g', Scalar>();
    parser.add_argument("--ray_error_abs")
        .default_value<Scalar>(1e-8)
        .help(
            "upper bound for absolute ray integration error (lower is better)")
        .scan<'g', Scalar>();
    parser.add_argument("--ray_error_rel")
        .default_value<Scalar>(1e-8)
        .help(
            "upper bound for relative ray integration error (lower is better)")
        .scan<'g', Scalar>();
    parser.add_argument("--ray_max_length")
        .default_value<Scalar>(1e+8)
        .help("upper bound for total ray length (larger is better)")
        .scan<'g', Scalar>();
    parser.add_argument("--ray_segment_length_factor")
        .default_value<Scalar>(1.1)
        .help("factor to multiply the length of each ray segment (shoulde be a "
              "bit larger then 1.0 to avoid small scale geometrical banding)")
        .scan<'g', Scalar>();

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
    config.shutter_mode = parser.get("--shutter_mode");
    config.exposure_time = parser.get<Scalar>("--exposure_time");
    config.total_line_exposure_time =
        parser.get<Scalar>("--total_line_exposure_time");
    config.gamma = parser.get<ColorScalar>("--gamma");
    config.debug_normals = parser.get<bool>("--debug_normals");
    config.debug_ray_terminations =
        parser.get<bool>("--debug_ray_terminations");
    config.speed_of_light = parser.get<Scalar>("--speed_of_light");
    config.schwarzschild_radius = parser.get<Scalar>("--schwarzschild_radius");
    config.ray_initial_step_size =
        parser.get<Scalar>("--ray_initial_step_size");
    config.ray_error_abs = parser.get<Scalar>("--ray_error_abs");
    config.ray_error_rel = parser.get<Scalar>("--ray_error_rel");
    config.ray_max_length = parser.get<Scalar>("--ray_max_length");
    config.ray_segment_length_factor =
        parser.get<Scalar>("--ray_segment_length_factor");

    render_ppm(config);
}
