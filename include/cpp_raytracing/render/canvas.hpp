/**
 * @file
 * @brief canvas
 */

#ifndef CPP_RAYTRACING_CANVAS_HPP
#define CPP_RAYTRACING_CANVAS_HPP

namespace cpp_raytracing {

/**
 * @brief represents a canvas
 */
struct Canvas {
    /** @brief default canvas width in pixel */
    constexpr static long DEFAULT_WIDTH = 128;
    /** @brief default canvas height in pixel */
    constexpr static long DEFAULT_HEIGHT = 128;

    /** @brief canvas width in pixel */
    unsigned long width = DEFAULT_WIDTH;
    /** @brief canvas height in pixel */
    unsigned long height = DEFAULT_HEIGHT;
};

} // namespace cpp_raytracing

#endif
