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

/**
 * @brief write image to ppm file
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

} // namespace cpp_raytracing::examples

#endif
