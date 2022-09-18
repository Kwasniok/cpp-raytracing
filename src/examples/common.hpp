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
 * @brief linear motion based triangle entity animator
 */
class LinearMotionTriangleAnimator : public TriangleAnimator {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::array<Vec3, 3> start_points;
    /** @brief constant velocity */
    Vec3 velocity;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    virtual ~LinearMotionTriangleAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time,
                                      Triangle* tri) override;
};

void LinearMotionTriangleAnimator::update_for_time_hook(const Scalar time,
                                                        Triangle* tri) {
    if (tri == nullptr)
        return;
    for (int i = 0; i < 3; ++i) {
        tri->points[i] = start_points[i] + (time - time_offset) * velocity;
    }
}

/**
 * @brief sinusoidal motion based triangle entity animator
 */
class SinusoidalMotionTriangleAnimator : public TriangleAnimator {
  public:
    /** @brief start position for `time = time_offset` of all points */
    std::array<Vec3, 3> start_points;
    /** @brief oscillator amplitude */
    Vec3 amplitude;
    /** @brief frequency of motion in radians */
    Scalar frequency = 0.0;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    virtual ~SinusoidalMotionTriangleAnimator() = default;

  protected:
    virtual void update_for_time_hook(const Scalar time,
                                      Triangle* tri) override;
};

void SinusoidalMotionTriangleAnimator::update_for_time_hook(const Scalar time,
                                                            Triangle* tri) {
    if (tri == nullptr)
        return;
    for (int i = 0; i < 3; ++i) {
        tri->points[i] = start_points[i] +
                         std::sin(frequency * (time - time_offset)) * amplitude;
    }
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
