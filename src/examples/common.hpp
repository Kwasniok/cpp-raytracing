/**
 * @file
 * @brief common header for all examples
 */

#ifndef CPP_RAYTRACING_EXAMPLES_COMMON_HPP
#define CPP_RAYTRACING_EXAMPLES_COMMON_HPP

#include <fstream>
#include <iostream>
#include <string>

#include <cpp_raytracing.hpp>

namespace cpp_raytracing::examples {

/** @brief global shutter mode constant */
constexpr std::string SHUTTER_MODE_GLOBAL_SHUTTER = "global";
/** @brief rolling shutter mode constant */
constexpr std::string SHUTTER_MODE_ROLLING_SHUTTER = "rolling";

/** @brief all shutter modes supported */
constexpr std::array<const std::string, 2> SHUTTER_MODES = {
    SHUTTER_MODE_GLOBAL_SHUTTER,
    SHUTTER_MODE_ROLLING_SHUTTER,
};

/**
 * @brief linear motion based mesh entity animator
 */
class LinearMotionMeshAnimator : public MeshAnimator {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::vector<Vec3> start_points;
    /** @brief constant velocity */
    Vec3 velocity;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    virtual ~LinearMotionMeshAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time, Mesh* mesh) override;
};

void LinearMotionMeshAnimator::update_for_time_hook(const Scalar time,
                                                    Mesh* mesh) {
    if (mesh == nullptr)
        return;
    mesh->points = start_points;
    for (auto& point : mesh->points) {
        point = point + velocity * (time - time_offset);
    }
}

/**
 * @brief sinusoidal motion based mesh entity animator
 */
class SinusoidalMotionMeshAnimator : public MeshAnimator {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::vector<Vec3> start_points;
    /** @brief oscillator amplitude */
    Vec3 amplitude;
    /** @brief frequency of motion in radians */
    Scalar frequency = 0.0;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    virtual ~SinusoidalMotionMeshAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time, Mesh* mesh) override;
};

void SinusoidalMotionMeshAnimator::update_for_time_hook(const Scalar time,
                                                        Mesh* mesh) {
    if (mesh == nullptr)
        return;

    mesh->points = start_points;
    for (auto& point : mesh->points) {
        point = point + std::sin(frequency * (time - time_offset)) * amplitude;
    }
}

/**
 * @brief returns a cube as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh> make_cube(const Scalar scale, const Vec3& position) {
    auto mesh = std::make_shared<Mesh>();
    mesh->id.change("cube");
    mesh->points = {
        scale * Vec3{-1.0, -1.0, -1.0} + position, // 0
        scale * Vec3{-1.0, -1.0, +1.0} + position, // 1
        scale * Vec3{-1.0, +1.0, -1.0} + position, // 2
        scale * Vec3{-1.0, +1.0, +1.0} + position, // 3
        scale * Vec3{+1.0, -1.0, -1.0} + position, // 4
        scale * Vec3{+1.0, -1.0, +1.0} + position, // 5
        scale * Vec3{+1.0, +1.0, -1.0} + position, // 6
        scale * Vec3{+1.0, +1.0, +1.0} + position, // 7
    };
    mesh->faces = {
        Face{0, 1, 3}, Face{3, 2, 0}, // -x
        Face{5, 4, 6}, Face{6, 7, 5}, // +x
        Face{1, 0, 4}, Face{4, 5, 1}, // -y
        Face{2, 3, 7}, Face{7, 6, 2}, // +y
        Face{4, 0, 2}, Face{2, 6, 4}, // -z
        Face{1, 5, 7}, Face{7, 3, 1}, // +z
    };

    return mesh;
}

/**
 * @brief returns a bounded x-z plane as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh> make_xz_plane(const Scalar scale, const Vec3& position) {
    auto mesh = std::make_shared<Mesh>();
    mesh->id.change("cube");
    mesh->points = {
        scale * Vec3{-1.0, 0.0, -1.0} + position, // 0
        scale * Vec3{-1.0, 0.0, +1.0} + position, // 1
        scale * Vec3{+1.0, 0.0, -1.0} + position, // 2
        scale * Vec3{+1.0, 0.0, +1.0} + position, // 3
    };
    mesh->faces = {Face{0, 1, 3}, Face{3, 2, 0}};

    return mesh;
}

/** @brief returns diffuse material */
std::shared_ptr<Material> make_diffuse_material(const Color& color) {
    auto mat = std::make_shared<Diffuse>();
    auto texture = std::make_shared<ConstantColor>();
    texture->color = color;
    mat->color = std::move(texture);
    return mat;
}

/** @brief returns diffuse 3D checker board material */
std::shared_ptr<Material>
make_diffuse_3d_checker_material(const Color& color1, const Color& color2,
                                 const Scalar scale = 1.0,
                                 const Vec3& offset = {0.0, 0.0, 0.0}) {
    auto mat = std::make_shared<Diffuse>();
    auto texture = std::make_shared<Checker3D>();
    texture->color1 = color1;
    texture->color2 = color2;
    texture->scale = scale;
    texture->offset = offset;
    mat->color = std::move(texture);
    return mat;
}

/** @brief returns light emitting material */
std::shared_ptr<Material>
make_light_material(const Color& color, const ColorScalar strength = 1.0) {
    auto mat = std::make_shared<Emitter>();
    auto texture = std::make_shared<ConstantColor>();
    texture->color = strength * color;
    mat->color = std::move(texture);
    return mat;
}

/**
 * @brief write image to ppm file
 * @note The ppm file format is lossy.
 * @param path path to ppm file (without extension)
 * @param image raw image to be written
 * @param scale factor to multiply each channel's value with
 * @param gamma gamma correction
 */
void write_ppm(const std::string& path, const RawImage& image,
               const ColorScalar scale, const ColorScalar gamma) {
    std::ofstream file;
    file.open(path + ".ppm");
    if (file) {
        write_image_ppm(file, image, scale, gamma);
    } else {
        std::cerr << "Could not open file " << path << std::endl;
    }
    file.close();
}

/**
 * @brief write raw image to pfm file
 * @note The pfm file format might be lossy, but is less lossy then e.g. ppm.
 * @note The pfm format supports 32-bit floats only, which might not be enough
 *       to represent a ColorScalar accurately.
 * @param path path to ppm file (without extension)
 * @param image raw image to be written
 * @param scale factor to multiply each channel's value with
 */
void write_pfm(const std::string& path, const RawImage& image,
               const ColorScalar scale) {
    std::ofstream file;
    file.open(path + ".pfm");
    if (file) {
        write_image_pfm(file, image, scale);
    } else {
        std::cerr << "Could not open file " << path << std::endl;
    }
    file.close();
}

/**
 * @brief write image to ppm and pfm file
 * @see write_ppm, write_pfm
 */
void write_image(const std::string& path, const RawImage& image,
                 const ColorScalar scale, const ColorScalar gamma) {
    write_ppm(path, image, scale, gamma);
    write_pfm(path, image, scale);
}

} // namespace cpp_raytracing::examples

#endif
