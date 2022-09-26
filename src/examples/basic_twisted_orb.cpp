/**
 * @file
 * @brief basic example executable in curved space ('twisted orb' geometry)
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    const Vec3 camera_position = 1.1 * Vec3{20, 12, 12};
    const Vec3 pinhole_position = 1.1 * Vec3{15, 8.4, 9};
    auto camera = std::make_shared<PinholeCamera>(cartesian_pinhole_camera(
        camera_position, pinhole_position, {0.0, 1.0, 0.0}, 30.0, 16.0 / 9.0));
    Scene scene(camera);

    // background (global illumination)
    {
        auto background = std::make_shared<SkyBackground>();
        scene.active_background = std::move(background);
    }

    // materials
    auto metal_gray = make_metal_3d_checker_material(Color{0.45, 0.45, 0.45},
                                                     Color{0.55, 0.55, 0.55});
    metal_gray->id.change("metal gray");

    auto diffuse_red = make_diffuse_material(Color{0.75, 0.5, 0.5});
    diffuse_red->id.change("diffuse red");

    // cube
    {
        auto cube = make_cube(1.0, Vec3{0.0, 0.0, 0.0});
        cube->material = diffuse_red;
        // animation
        auto anim = std::make_unique<SinusoidalMotionMeshAnimator>();
        anim->start_points = cube->points;
        anim->amplitude = {5.0, 0.0, 2.0};
        anim->frequency = pi;
        cube->set_animator(std::move(anim));
        scene.add(std::move(cube));
    }

    // floor
    {
        auto plane = make_xz_plane(1e4, Vec3{0.0, -1.0, 0.0});
        plane->id.change("floor");
        plane->material = metal_gray;
        scene.add(std::move(plane));
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
    /** @brief shutter mode */
    std::string shutter_mode;
    /** @brief exposure frame */
    Scalar exposure_time;
    /** @brief exposure line */
    Scalar total_line_exposure_time;
    /** @brief gamma correction for non-raw images */
    ColorScalar gamma;
    /** @brief debug normals */
    bool debug_normals;
    /** @brief debug premature ray termination */
    bool debug_ray_terminations;
    /** @brief strength of geometric twist effect */
    Scalar twist_angle;
    /** @brief spatial extend of geoemtric twist effect */
    Scalar twist_radius;
    /** @brief size parameter for ray segments */
    Scalar ray_step_size;
    /** @brief minimal size parameter for ray segments */
    Scalar ray_min_step_size;
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

    TwistedOrbCartesianGeometry geometry{
        config.twist_angle, config.twist_radius, config.ray_step_size,
        config.ray_min_step_size};
    Scene scene = make_scene();

    std::unique_ptr<Renderer> renderer;

    if (config.shutter_mode == SHUTTER_MODE_GLOBAL_SHUTTER) {
        auto rendr = std::make_unique<GlobalShutterRenderer>();
        rendr->exposure_time = config.exposure_time;
        renderer = std::move(rendr);
    }
    if (config.shutter_mode == SHUTTER_MODE_ROLLING_SHUTTER) {
        auto rendr = std::make_unique<RollingShutterRenderer>();
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
        [](const Renderer::State& current_state) {
            cout << "samples: " << current_state.samples << endl;
        };

    renderer->infrequent_render_callback =
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
    RawImage image = renderer->render(geometry, scene);
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
        .default_value<unsigned long>(5)
        .help("samples per pixel")
        .scan<'d', unsigned long>();
    parser.add_argument("--save_frequency")
        .default_value<unsigned long>(10)
        .help("save progress every n samples")
        .scan<'d', unsigned long>();
    parser.add_argument("--ray_depth")
        .default_value<unsigned long>(250)
        .help("depth per ray (amount of ray segments and scatterings)")
        .scan<'d', unsigned long>();
    parser.add_argument("--time")
        .default_value<Scalar>(0.0)
        .help("time of the frame")
        .scan<'f', Scalar>();
    parser.add_argument("--shutter_mode")
        .default_value(SHUTTER_MODE_GLOBAL_SHUTTER)
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
        .scan<'f', Scalar>();
    parser.add_argument("--total_line_exposure_time")
        .default_value<Scalar>(0.0)
        .help("exposure time per line (rolling shutter mode only)")
        .scan<'f', Scalar>();
    parser.add_argument("--gamma")
        .default_value<ColorScalar>(2.0)
        .help("gamma correction for non-raw image formats")
        .scan<'f', ColorScalar>();
    parser.add_argument("--debug_normals")
        .default_value<bool>(false) // store_true
        .implicit_value(true)
        .help("enable render mode to debug surface normals");
    parser.add_argument("--debug_ray_terminations")
        .default_value<bool>(false) // store_true
        .implicit_value(true)
        .help("enable to highlight pixels of prematurely ended rays");
    parser.add_argument("--twist_angle")
        .default_value<Scalar>(-pi / 2)
        .help("strength of geometric twist effect in radians (max twist angle, "
              "0.0 is flat space)")
        .scan<'f', Scalar>();
    parser.add_argument("--twist_radius")
        .default_value<Scalar>(2.0)
        .help("sptial extend of twisting")
        .scan<'f', Scalar>();
    parser.add_argument("--ray_step_size")
        .default_value<Scalar>(1e-1)
        .help("influences length of ray segments")
        .scan<'f', Scalar>();
    parser.add_argument("--ray_min_step_size")
        .default_value<Scalar>(1e-4)
        .help("lower bound for length of ray segments")
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
    config.shutter_mode = parser.get("--shutter_mode");
    config.exposure_time = parser.get<Scalar>("--exposure_time");
    config.total_line_exposure_time =
        parser.get<Scalar>("--total_line_exposure_time");
    config.gamma = parser.get<ColorScalar>("--gamma");
    config.debug_normals = parser.get<bool>("--debug_normals");
    config.debug_ray_terminations =
        parser.get<bool>("--debug_ray_terminations");
    config.twist_angle = parser.get<Scalar>("--twist_angle");
    config.twist_radius = parser.get<Scalar>("--twist_radius");
    config.ray_step_size = parser.get<Scalar>("--ray_step_size");
    config.ray_min_step_size = parser.get<Scalar>("--ray_min_step_size");

    if (config.ray_step_size > 1.0 && config.twist_angle != 0.0) {
        std::cerr << "WARNING: ray_step_size > 1.0 is not recommended!"
                  << std::endl;
    }
    if (config.ray_step_size < config.ray_min_step_size) {
        std::cerr << "ERROR: ray_step_size < ray_min_step_size is not "
                     "sensible!"
                  << std::endl;
        std::exit(1);
    }

    render_ppm(config);
}
