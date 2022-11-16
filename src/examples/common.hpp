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
const std::string SHUTTER_MODE_GLOBAL = "global";
/** @brief rolling shutter mode constant */
const std::string SHUTTER_MODE_ROLLING = "rolling";

/** @brief all shutter modes supported */
const auto SHUTTER_MODES = std::to_array({
    SHUTTER_MODE_GLOBAL,
    SHUTTER_MODE_ROLLING,
});

/**
 * @brief linear motion based mesh entity animator
 */
class LinearMotionMeshAnimator3D : public MeshAnimator3D {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::vector<Vec3> start_points;
    /** @brief constant velocity */
    Vec3 velocity;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    /** @brief default constructor */
    LinearMotionMeshAnimator3D() = default;

    /** @brief copy constructor */
    LinearMotionMeshAnimator3D(const LinearMotionMeshAnimator3D&) = delete;

    /** @brief move constructor */
    LinearMotionMeshAnimator3D(LinearMotionMeshAnimator3D&&) = default;

    /** @brief copy assignment */
    LinearMotionMeshAnimator3D&
    operator=(const LinearMotionMeshAnimator3D&) = delete;

    /** @brief move assignment */
    LinearMotionMeshAnimator3D&
    operator=(LinearMotionMeshAnimator3D&&) = default;

    ~LinearMotionMeshAnimator3D() override = default;

  protected:
    /** @see MeshAnimator3D::update_for_time_hook */
    void update_for_time_hook(const Scalar time, Mesh3D* mesh) override;
};

void LinearMotionMeshAnimator3D::update_for_time_hook(const Scalar time,
                                                      Mesh3D* mesh) {
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
class SinusoidalMotionMeshAnimator3D : public MeshAnimator3D {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::vector<Vec3> start_points;
    /** @brief oscillator amplitude */
    Vec3 amplitude;
    /** @brief frequency of motion in radians */
    Scalar frequency = 0.0;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    /** @brief default constructor */
    SinusoidalMotionMeshAnimator3D() = default;

    /** @brief copy constructor */
    SinusoidalMotionMeshAnimator3D(const SinusoidalMotionMeshAnimator3D&) =
        delete;

    /** @brief move constructor */
    SinusoidalMotionMeshAnimator3D(SinusoidalMotionMeshAnimator3D&&) = default;

    /** @brief copy assignment */
    SinusoidalMotionMeshAnimator3D&
    operator=(const SinusoidalMotionMeshAnimator3D&) = delete;

    /** @brief move assignment */
    SinusoidalMotionMeshAnimator3D&
    operator=(SinusoidalMotionMeshAnimator3D&&) = default;

    ~SinusoidalMotionMeshAnimator3D() override = default;

  protected:
    /** @see MeshAnimator3D::update_for_time_hook */
    void update_for_time_hook(const Scalar time, Mesh3D* mesh) override;
};

void SinusoidalMotionMeshAnimator3D::update_for_time_hook(const Scalar time,
                                                          Mesh3D* mesh) {
    if (mesh == nullptr)
        return;

    mesh->points = start_points;
    for (auto& point : mesh->points) {
        point = point + std::sin(frequency * (time - time_offset)) * amplitude;
    }
}

/**
 * @brief simple sky background (requires Cartesian coorinates)
 */
class SkyBackground3D : public Background3D {
  public:
    /** @brief color near horizon */
    Color color1 = Colors::WHITE;
    /** @brief color near horizon */
    Color color2 = {0.5, 0.7, 1.0};

    /** @brief default constructor */
    SkyBackground3D() = default;

    /** @brief copy constructor */
    SkyBackground3D(const SkyBackground3D&) = delete;

    /** @brief move constructor */
    SkyBackground3D(SkyBackground3D&&) = default;

    /** @brief copy assignment */
    SkyBackground3D& operator=(const SkyBackground3D&) = delete;

    /** @brief move assignment */
    SkyBackground3D& operator=(SkyBackground3D&&) = default;

    ~SkyBackground3D() override = default;

    /** @see Background::value */
    Color value([[maybe_unused]] const Geometry3D& geometry,
                const RaySegment3D& ray_segment) const override {

        using namespace tensor;

        const Vec3 direction = unit_vector(ray_segment.direction());
        const auto t = 0.5 * (std::abs(direction[1]) + 1.0);
        const Color color = (1.0 - t) * color1 + t * color2;
        return color;
    };
};

/**
 * @brief returns a cube as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh3D> make_cube(const Scalar scale, const Vec3& position) {
    auto mesh = std::make_shared<SmallTriangleMesh3D>();
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
        Face3D{0, 1, 3}, Face3D{3, 2, 0}, // -x
        Face3D{5, 4, 6}, Face3D{6, 7, 5}, // +x
        Face3D{1, 0, 4}, Face3D{4, 5, 1}, // -y
        Face3D{2, 3, 7}, Face3D{7, 6, 2}, // +y
        Face3D{4, 0, 2}, Face3D{2, 6, 4}, // -z
        Face3D{1, 5, 7}, Face3D{7, 3, 1}, // +z
    };

    return mesh;
}

/**
 * @brief returns a bounded x-z plane as mesh entity
 * @note Uses Cartesian coordinates.
 */
std::shared_ptr<Mesh3D> make_xz_plane(const Scalar scale,
                                      const Vec3& position) {
    auto mesh = std::make_shared<SmallTriangleMesh3D>();
    mesh->id.change("plane");
    mesh->points = {
        scale * Vec3{-1.0, 0.0, -1.0} + position, // 0
        scale * Vec3{-1.0, 0.0, +1.0} + position, // 1
        scale * Vec3{+1.0, 0.0, -1.0} + position, // 2
        scale * Vec3{+1.0, 0.0, +1.0} + position, // 3
    };
    mesh->faces = {Face3D{0, 1, 3}, Face3D{3, 2, 0}};

    return mesh;
}

/** @brief returns constant color texture */
std::shared_ptr<Texture3D> make_color_texture(const Color& color) {
    auto texture = std::make_shared<ConstantColor3D>();
    texture->color = color;
    return texture;
}

/** @brief returns texture with 3D checker board pattern */
std::shared_ptr<Texture3D>
make_checker_3d_texture(const Color& color1, const Color& color2,
                        const Scalar scale = 1.0,
                        const Vec3& offset = {0.0, 0.0, 0.0}) {
    auto texture = std::make_shared<VolumeChecker3D>();
    texture->color1 = color1;
    texture->color2 = color2;
    texture->scale = scale;
    texture->offset = offset;
    return texture;
}

/** @brief returns diffuse material */
std::shared_ptr<Material3D> make_diffuse_material(const Color& color) {
    auto mat = std::make_shared<Diffuse3D>();
    auto texture = std::make_shared<ConstantColor3D>();
    mat->color = make_color_texture(color);
    return mat;
}

/** @brief returns diffuse 3D checker board material */
std::shared_ptr<Material3D>
make_diffuse_3d_checker_material(const Color& color1, const Color& color2,
                                 const Scalar scale = 1.0,
                                 const Vec3& offset = {0.0, 0.0, 0.0}) {
    auto mat = std::make_shared<Diffuse3D>();
    mat->color = make_checker_3d_texture(color1, color2, scale, offset);
    return mat;
}

/** @brief returns metal material */
std::shared_ptr<Material3D>
make_metal_material(const Color& color, const ColorScalar roughness = 0.0) {
    auto mat = std::make_shared<Metal3D>();
    mat->color = make_color_texture(color);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns metal 3D checker board material */
std::shared_ptr<Material3D> make_metal_3d_checker_material(
    const Color& color1, const Color& color2, const Scalar roughness = 0.0,
    const Scalar scale = 1.0, const Vec3& offset = {0.0, 0.0, 0.0}) {
    auto mat = std::make_shared<Metal3D>();
    mat->color = make_checker_3d_texture(color1, color2, scale, offset);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns light emitting material */
std::shared_ptr<Material3D>
make_light_material(const Color& color, const ColorScalar strength = 1.0) {
    auto mat = std::make_shared<Emitter3D>();
    mat->color = make_color_texture(strength * color);
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
