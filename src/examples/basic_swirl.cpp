/**
 * @file
 * @brief basic example executable in curved space ('swirl' geometry)
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common_cartesian_3d.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

/**
 * @brief generate an example scene
 */
Scene3D make_scene() {

    auto camera = std::make_shared<PinholeCamera3D>(
        make_pinhole_camera_cartesian_euclidean_3d(
            {1.5, 2.0, 2.5}, {1.0, 1.5, 2.0}, {0.0, 1.0, 0.0}, 90.0,
            16.0 / 9.0));
    Scene3D scene(camera);

    // background (global illumination)
    {
        auto background = std::make_shared<ConstantBackground3D>();
        background->color = Color{0.5, 0.7, 1.0};
        scene.active_background = std::move(background);
    }

    // materials
    auto diffuse_gray = make_diffuse_volume_checker_material<3_D>(
        Color{0.45, 0.45, 0.45}, Color{0.55, 0.55, 0.55});
    diffuse_gray->id.change("diffuse gray");

    auto diffuse_red = make_diffuse_material<3_D>(Color{0.75, 0.5, 0.5});
    diffuse_red->id.change("diffuse red");

    // cube
    {
        auto cube = make_cube_3d(1.0, Vec3{0.0, 0.0, 0.0});
        cube->material = diffuse_red;
        // animation
        auto anim = std::make_unique<SinusoidalMotionMeshAnimator3D>();
        anim->start_points = cube->points;
        anim->amplitude = {5.0, 0.0, 2.0};
        anim->frequency = pi;
        cube->set_animator(std::move(anim));
        scene.add(std::move(cube));
    }

    // floor
    {
        auto plane = make_xz_plane(1e4, Vec3{0.0, -1.0- 1e-8, 0.0});
        plane->id.change("floor");
        plane->material = diffuse_gray;
        scene.add(std::move(plane));
    }

    return scene;
}

/** @brief configuration for render_ppm */
struct RenderConfig {
    /** @brief wheather to log detailed information during the render process */
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
    /** @brief strength of geometric swirl effect */
    Scalar swirl_strength = 0.0;
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

    SwirlCartesianGeometry3D geometry{
        config.swirl_strength, config.ray_initial_step_size,
        config.ray_error_abs,  config.ray_error_rel,
        config.ray_max_length, config.ray_segment_length_factor,
    };
    Scene3D scene = make_scene();

    std::unique_ptr<Renderer3D> renderer;

    if (config.shutter_mode == SHUTTER_MODE_GLOBAL) {
        auto rendr = std::make_unique<GlobalShutterRenderer3D>();
        rendr->exposure_time = config.exposure_time;
        renderer = std::move(rendr);
    }
    if (config.shutter_mode == SHUTTER_MODE_ROLLING) {
        auto rendr = std::make_unique<RollingShutterRenderer3D>();
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
        [](const Renderer3D::State& current_state) {
            cout << "samples: " << current_state.samples << endl;
        };

    renderer->infrequent_render_callback =
        [&config](const Renderer3D::State& current_state) {
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
        .default_value<unsigned long>(100) // NOLINT
        .help("depth per ray (amount of ray segments and scatterings)")
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
        .implicit_value(true)
        .help("enable to highlight pixels of prematurely ended rays");
    parser.add_argument("--swirl_strength")
        .default_value<Scalar>(0.01)
        .help("strength of geometric swirl effect (0.0 is flat space)")
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
    config.swirl_strength = parser.get<Scalar>("--swirl_strength");
    config.ray_initial_step_size =
        parser.get<Scalar>("--ray_initial_step_size");
    config.ray_error_abs = parser.get<Scalar>("--ray_error_abs");
    config.ray_error_rel = parser.get<Scalar>("--ray_error_rel");
    config.ray_max_length = parser.get<Scalar>("--ray_max_length");
    config.ray_segment_length_factor =
        parser.get<Scalar>("--ray_segment_length_factor");

    render_ppm(config);
}
