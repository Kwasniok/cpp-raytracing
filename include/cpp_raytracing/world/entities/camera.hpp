/**
 * @file
 * @brief camera
 */

#ifndef CPP_RAYTRACING_CAMERA_HPP
#define CPP_RAYTRACING_CAMERA_HPP

#include <memory>

#include "../../geometry/base.hpp"
#include "../../values/tensor.hpp"
#include "../ray.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief represents camera interface
 */
class Camera : public Entity {
  public:
    /**
     * @brief calculates ray for canvas coordinates
     * @param geometry geometry of space
     * @param x width parameter `-1..+1`
     * @param y height parameter `-1..+1`
     */
    virtual std::unique_ptr<Ray> ray_for_coords(const Geometry& geometry,
                                                const Scalar x,
                                                const Scalar y) const = 0;
};

/**
 * @brief represents a pinhole camera
 */
class PinholeCamera : public Camera {
  public:
    /**
     * @brief representation of detector surface as a char
     * @see ray_for_coords
     */
    using DetectorSurface = std::function<Vec3(const Scalar, const Scalar)>;

    /** @brief manifold for detector surface */
    DetectorSurface detector_surface;
    /** @brief position of pinhole */
    Vec3 pinhole;

    /**
     * @brief construct pinhole camera
     * @param pinhole pinhole position
     * @param surface_arg used to forward initialize detector_surface
     */
    template <class T>
    PinholeCamera(const Vec3& pinhole, T surface_arg)
        : detector_surface(std::forward<T>(surface_arg)), pinhole(pinhole) {}

    virtual std::unique_ptr<Ray> ray_for_coords(const Geometry& geometry,
                                                const Scalar x,
                                                const Scalar y) const override {
        const Vec3 start = detector_surface(x, y);
        return geometry.ray_passing_through(start, pinhole);
    }
};

} // namespace cpp_raytracing

#endif
