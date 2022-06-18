/**
 * @file
 * @brief complex example executable
 * @note Comlpex scene.
 */

#include <argparse/argparse.hpp>
#include <iostream>
#include <memory>

#include "common.hpp"

using namespace std;
using namespace cpp_raytracing;
using namespace cpp_raytracing::examples;

/**
 * @brief linear motion based instance animator
 */
class LinearMotionInstanceAnimator : public InstanceAnimator {
  public:
    /** @brief start position for `time = time_offset` */
    Vec3 start;
    /** @brief constant velocity */
    Vec3 velocity;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    virtual ~LinearMotionInstanceAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time,
                                      Instance* object) override;
};

void LinearMotionInstanceAnimator::update_for_time_hook(const Scalar time,
                                                        Instance* object) {
    if (object == nullptr)
        return;
    object->position = start + (time - time_offset) * velocity;
}

/**
 * @brief spiral motion based instance animator
 */
class SpiralMotionInstanceAnimator : public InstanceAnimator {
  public:
    /** @brief start position for `time = time_offset` */
    Vec3 start;
    /** @brief radius of circular part of motion */
    Scalar radius;
    /** @brief frequency of circular motion (in radians) */
    Scalar frequency = 1.0;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;
    /** @brief phase of circular motion for `time = time_offset` */
    Scalar phase_offset = 0.0;

    virtual ~SpiralMotionInstanceAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time,
                                      Instance* object) override;
};

void SpiralMotionInstanceAnimator::update_for_time_hook(const Scalar time,
                                                        Instance* object) {
    if (object == nullptr)
        return;
    const Scalar t = time - time_offset;
    const Scalar phi = frequency * t - phase_offset;
    const Scalar cos_phi = std::cos(phi);
    const Scalar sin_phi = std::sin(phi);
    object->position = start + (t * radius) * Vec3{cos_phi, sin_phi, 0.0};
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

/** @brief generate a plane instance */
std::shared_ptr<Instance> make_plane(const std::shared_ptr<Material>& material,
                                     const bool finite = true) {
    auto plane = std::make_shared<Plane>();
    plane->material = material;
    plane->finite_neg_x = finite;
    plane->finite_pos_x = finite;
    plane->finite_neg_y = finite;
    plane->finite_pos_y = finite;

    auto instance = std::make_shared<Instance>();
    instance->entity = plane;

    return instance;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    Scene scene;
    scene.active_camera = std::make_shared<Camera>(
        Camera::from({0.0, 1.0, 0.0}, {0.0, 0.0, -5.0}, {0.0, 1.0, 0.0},
                     // note: The field of view had a bug originally and this
                     //       number is for backwards-compatibility.
                     22.61986495, // = artan(tan(90 / 2) / 5) * 2
                     16.0 / 9.0, 0.02));

    const int num_material_variations = 250;
    const int num_spheres = 1000;

    // list of random materials
    std::vector<std::shared_ptr<Material>> materials;
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_shared<Diffuse>();
        mat->id.change("diffuse");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = Color::random(0.0, 1.0);
        mat->color = std::move(texture);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_shared<Metal>();
        mat->id.change("metal");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = Color::random(0.6, 1.0);
        mat->color = std::move(texture);
        mat->roughness = random_scalar(0.0, 1.0);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_shared<Dielectric>();
        mat->id.change("glass");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = Color::random(0.7, 1.0);
        mat->color = std::move(texture);
        mat->index_of_refraction = random_scalar(1.0, 2.5);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_shared<Emitter>();
        mat->id.change("light");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = Color::random(0.7, 1.0);
        mat->color = std::move(texture);
        materials.emplace_back(std::move(mat));
    }

    // random small spheres
    for (int i = 0; i < num_spheres; ++i) {
        const auto y = random_scalar(0.05, 0.15);
        const auto z = random_scalar(-100.0, -1.0);
        const auto x = std::abs(z) / 10.0 * random_scalar(-5.0, +5.0);
        const auto radius = std::abs(y);
        auto sphere = make_sphere(radius, materials[rand() % materials.size()]);
        sphere->id.change("sphere");
        sphere->position = Vec3(x, y, z);

        // random animation per object
        if (rand() % 2) {
            auto anim = make_unique<LinearMotionInstanceAnimator>();
            {
                anim->start = {sphere->position};
                const auto x = random_scalar(-1.0, +1.0);
                const auto y = random_scalar(0.0, +1.0);
                const auto z = 0.0;
                anim->velocity = {x, y, z};
            }
            sphere->set_animator(std::move(anim));
        } else {
            auto anim = make_unique<SpiralMotionInstanceAnimator>();
            {
                anim->start = sphere->position;
                anim->radius = radius;
                anim->frequency = random_scalar(-1.0, 1.0);
            }
            sphere->set_animator(std::move(anim));
        }
        scene.add(std::move(sphere));
    }

    // floor
    {
        auto mat = std::make_shared<Metal>();
        mat->id.change("floor");
        auto texture = std::make_shared<ConstantColor>();
        texture->color = {0.7, 0.8, 0.9};
        mat->color = std::move(texture);
        mat->roughness = 0.3;
        auto plane = make_plane(std::move(mat), false);
        plane->id.change("floor");
        plane->scale = Vec3(1e5, 1e5, 1.0);
        plane->rotation = Vec3(pi / 2.0, 0.0, 0.0);
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

    Scene scene = make_scene();

    RollingShutterRenderer renderer;
    renderer.canvas = canvas;
    renderer.samples = config.samples;
    renderer.ray_depth = config.ray_depth;
    renderer.exposure_time = 0.5;
    renderer.motion_blur = 0.01;
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
    argparse::ArgumentParser parser("complex");
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

    render_ppm(config);
}
