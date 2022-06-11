/**
 * @file
 * @brief complex example executable
 * @note Comlpex scene.
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
std::unique_ptr<Instance>
make_sphere(const Scalar radius, const std::shared_ptr<Material>& material) {
    auto sphere = std::make_shared_for_overwrite<Sphere>();
    sphere->radius = radius;
    sphere->material = material;

    auto instance = std::make_unique_for_overwrite<Instance>();
    instance->entity = sphere;

    return instance;
}

/** @brief generate a plane instance */
std::unique_ptr<Instance> make_plane(const std::shared_ptr<Material>& material,
                                     const bool finite = true) {
    auto plane = std::make_shared_for_overwrite<Plane>();
    plane->material = material;
    plane->finite_neg_x = finite;
    plane->finite_pos_x = finite;
    plane->finite_neg_y = finite;
    plane->finite_pos_y = finite;

    auto instance = std::make_unique_for_overwrite<Instance>();
    instance->entity = plane;

    return instance;
}

/**
 * @brief generate an example scene
 */
Scene make_scene() {

    Scene scene;
    scene.active_camera = std::make_unique<Camera>(
        Camera::from({0.0, 1.0, 0.0}, {0.0, 0.0, -5.0}, {0.0, 1.0, 0.0}, 90,
                     16.0 / 9.0, 0.02));

    const int num_material_variations = 250;
    const int num_spheres = 1000;

    // list of random materials
    std::vector<std::shared_ptr<Material>> materials;
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Diffuse>();
        mat->id.change("diffuse");
        mat->color = Color::random(0.0, 1.0);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Metal>();
        mat->id.change("metal");
        mat->color = Color::random(0.6, 1.0);
        mat->roughness = random_scalar(0.0, 1.0);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Dielectric>();
        mat->id.change("glass");
        mat->color = Color::random(0.7, 1.0);
        mat->index_of_refraction = random_scalar(1.0, 2.5);
        materials.emplace_back(std::move(mat));
    }
    for (int i = 0; i < num_material_variations; ++i) {
        auto mat = std::make_unique_for_overwrite<Emitter>();
        mat->id.change("light");
        mat->color = Color::random(0.7, 1.0);
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
        auto mat = std::make_unique_for_overwrite<Metal>();
        mat->id.change("floor");
        mat->color = {0.7, 0.8, 0.9};
        mat->roughness = 0.3;
        auto plane = make_plane(std::move(mat), false);
        plane->id.change("floor");
        plane->scale = Vec3(1e5, 1e5, 1.0);
        plane->rotation = Vec3(pi / 2.0, 0.0, 0.0);
        scene.add(std::move(plane));
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
    argparse::ArgumentParser parser("complex");
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
