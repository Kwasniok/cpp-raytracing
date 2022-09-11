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
    /** @brief canvas width in pixel */
    unsigned long width = 128;
    /** @brief canvas height in pixel */
    unsigned long height = 128;
};

} // namespace cpp_raytracing

#endif
