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

#include "cartesian_sky_background.hpp"
#include "linear_motion_mesh_animator.hpp"
#include "sinusoidal_motion_mesh_animator.hpp"

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

/** @brief returns constant color texture */
template <Dimension DIMENSION>
std::shared_ptr<Texture<DIMENSION>> make_color_texture(const Color& color) {
    auto texture = std::make_shared<ConstantColor<DIMENSION>>();
    texture->color = color;
    return texture;
}

/** @brief returns texture with volume checker board pattern */
template <Dimension DIMENSION>
std::shared_ptr<Texture<DIMENSION>>
make_volume_checker_texture(const Color& color1, const Color& color2,
                            const Scalar scale = 1.0,
                            const Vec<DIMENSION>& offset = {}) {
    auto texture = std::make_shared<VolumeChecker<DIMENSION>>();
    texture->color1 = color1;
    texture->color2 = color2;
    texture->scale = scale;
    texture->offset = offset;
    return texture;
}

/** @brief returns diffuse material */
template <Dimension DIMENSION>
std::shared_ptr<Material<DIMENSION>> make_diffuse_material(const Color& color) {
    auto mat = std::make_shared<Diffuse<DIMENSION>>();
    auto texture = std::make_shared<ConstantColor<DIMENSION>>();
    mat->color = make_color_texture<DIMENSION>(color);
    return mat;
}

/** @brief returns diffuse volume checker board material */
template <Dimension DIMENSION>
std::shared_ptr<Material<DIMENSION>>
make_diffuse_volume_checker_material(const Color& color1, const Color& color2,
                                     const Scalar scale = 1.0,
                                     const Vec<DIMENSION>& offset = {}) {
    auto mat = std::make_shared<Diffuse<DIMENSION>>();
    mat->color =
        make_volume_checker_texture<DIMENSION>(color1, color2, scale, offset);
    return mat;
}

/** @brief returns metal material */
template <Dimension DIMENSION>
std::shared_ptr<Material<DIMENSION>>
make_metal_material(const Color& color, const ColorScalar roughness = 0.0) {
    auto mat = std::make_shared<Metal<DIMENSION>>();
    mat->color = make_color_texture<DIMENSION>(color);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns metal volume checker board material */
template <Dimension DIMENSION>
std::shared_ptr<Material<DIMENSION>> make_metal_volume_checker_material(
    const Color& color1, const Color& color2, const Scalar roughness = 0.0,
    const Scalar scale = 1.0, const Vec<DIMENSION>& offset = {}) {
    auto mat = std::make_shared<Metal<DIMENSION>>();
    mat->color =
        make_volume_checker_texture<DIMENSION>(color1, color2, scale, offset);
    mat->roughness = roughness;
    return mat;
}

/** @brief returns light emitting material */
template <Dimension DIMENSION>
std::shared_ptr<Material<DIMENSION>>
make_light_material(const Color& color, const ColorScalar strength = 1.0) {
    auto mat = std::make_shared<Emitter<DIMENSION>>();
    mat->color = make_color_texture<DIMENSION>(strength * color);
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
