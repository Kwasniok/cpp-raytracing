/**
 * @file
 * @brief demonstration of rolling shutter
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

/**
 * @brief circular motion based instance animator
 */
class CircularMotionInstanceAnimator : public InstanceAnimator {
  public:
    /** @brief start position for `time = time_offset` */
    Vec3 center;
    /** @brief radius of circular part of motion */
    Scalar radius;
    /** @brief frequency of circular motion (in radians) */
    Scalar frequency = 1.0;
    /** @brief phase of circular motion for `time = time_offset` */
    Scalar phase_offset = 0.0;

    virtual ~CircularMotionInstanceAnimator() = default;

  protected:
    /** @brief hook for update_for_time*/
    virtual void update_for_time_hook(const Scalar time,
                                      Instance* instance) override;
};

void CircularMotionInstanceAnimator::update_for_time_hook(const Scalar time,
                                                          Instance* instance) {
    if (instance == nullptr)
        return;
    const Scalar phi = frequency * time - phase_offset;
    const Scalar cos_phi = std::cos(phi);
    const Scalar sin_phi = std::sin(phi);
    instance->position = center + radius * Vec3{cos_phi, sin_phi, 0.0};
}

/** @brief convert HSV to color */
Color color_from_hsv(const ColorScalar hue, const ColorScalar saturation,
                     const ColorScalar value) {

    const ColorScalar c = value * saturation;
    const ColorScalar x =
        c * (1.0 - std::abs(std::fmod(hue * 3.0 / pi, 2.0) - 1.0));
    const ColorScalar m = value - c;

    ColorScalar r = m;
    ColorScalar g = m;
    ColorScalar b = m;

    static constexpr ColorScalar pie = 2.0 * pi / 6.0;
    if (hue < pie) {
        r += c;
        g += x;
    } else if (hue < 2.0 * pie) {
        r += x;
        g += c;
    } else if (hue < 3.0 * pie) {
        g += c;
        b += x;
    } else if (hue < 4.0 * pie) {
        g += x;
        b += c;
    } else if (hue < 5.0 * pie) {
        b += c;
        r += x;
    } else {
        b += x;
        r += c;
    }

    return {r, g, b};
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

/** @brief configuration of the scene */
struct SceneConfig {
    /** @brief number of blades in the rotor */
    unsigned int num_blades = 2;
    /** @brief frequency of blades */
    Scalar frequency = 1.0;
};

/**
 * @brief generate an example scene
 */
Scene make_scene(const SceneConfig& config) {

    const Vec3 center = {0.0, 1.5, -3.0};

    Scene scene;
    scene.active_camera = std::make_shared<Camera>(
        Camera::from({0.0, 1.5, 0.0}, {0.0, 1.5, -2.0}, {0.0, 1.0, 0.0},
                     // note: The field of view had a bug originally and this
                     //       number is for backwards-compatibility.
                     61.579466, // = artan(tan(100 / 2) / 2) * 2
                     16.0 / 9.0, 0.02));

    // rotor
    for (unsigned int i = 0; i < config.num_blades; ++i) {

        auto mat = std::make_shared<Diffuse>();
        mat->id.change("metal");
        const Color color = color_from_hsv(
            ColorScalar(i) / ColorScalar(config.num_blades) * 2.0 * pi, 0.8,
            0.8);

        auto texture = std::make_shared<ConstantColor>();
        texture->color = color;
        mat->color = std::move(texture);

        auto sphere = make_sphere(0.5, std::move(mat));
        sphere->id.change("sphere");
        sphere->position = center;

        auto anim = make_unique<CircularMotionInstanceAnimator>();
        {
            anim->center = center;
            anim->radius = 1.0;
            anim->frequency = config.frequency;
            anim->phase_offset = i * 2.0 * pi / config.num_blades;
        }
        sphere->set_animator(std::move(anim));

        scene.add(std::move(sphere));
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
    /** @brief exposure frame */
    Scalar exposure_time;
    /** @brief motion blur */
    Scalar motion_blur;
    /** @brief configuration of the scene */
    SceneConfig scene;
    /** @brief gamma correction for non-raw images */
    ColorScalar gamma;
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

    Scene scene = make_scene(config.scene);

    RollingShutterRenderer renderer;
    renderer.canvas = canvas;
    renderer.samples = config.samples;
    renderer.ray_depth = config.ray_depth;
    renderer.exposure_time = config.exposure_time;
    renderer.motion_blur = config.motion_blur;
    renderer.infrequent_callback_frequency = config.save_frequency;
    renderer.time = config.time;

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
    RawImage image = renderer.render(scene);
    write_image(config.path, image, 1.0, config.gamma);
}

/**
 * @brief program entry point
 */
int main(int argc, char** argv) {
    argparse::ArgumentParser parser("rolling_shutter");
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
    parser.add_argument("--exposure_time")
        .default_value<Scalar>(1.0)
        .help("total exposure time per frame")
        .scan<'f', Scalar>();
    parser.add_argument("--motion_blur")
        .default_value<Scalar>(0.0)
        .help("exposure time per line")
        .scan<'f', Scalar>();
    parser.add_argument("--frequency")
        .default_value<Scalar>(2.0 * pi)
        .help("frequency of the rotor")
        .scan<'f', Scalar>();
    parser.add_argument("--num_blades")
        .default_value<unsigned int>(5)
        .help("number of blades in the rotor (>=2)")
        .scan<'d', unsigned int>();

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
    config.exposure_time = parser.get<Scalar>("--exposure_time");
    config.motion_blur = parser.get<Scalar>("--motion_blur");
    config.scene.frequency = parser.get<Scalar>("--frequency");
    config.scene.num_blades = parser.get<unsigned int>("--num_blades");

    render_ppm(config);
}
