/**
 * @file
 * @brief camera
 */

#ifndef CPP_RAYTRACING_CAMERA_HPP
#define CPP_RAYTRACING_CAMERA_HPP

#include <memory>

#include "../../geometry/base.hpp"
#include "../../values/tensor.hpp"
#include "../ray_segment.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief represents camera interface
 */
template <Dimension DIMENSION>
class Camera : public Entity<DIMENSION> {
  public:
    /**
     * @brief calculates ray for canvas coordinates
     * @param geometry geometry of space
     * @param x width parameter `-1..+1`
     * @param y height parameter `-1..+1`
     */
    virtual std::unique_ptr<Ray<DIMENSION>>
    ray_for_coords(const Geometry<DIMENSION>& geometry, const Scalar x,
                   const Scalar y) const = 0;

    HitRecord<DIMENSION> hit_record(const Geometry<DIMENSION>&,
                                    const RaySegment<DIMENSION>&,
                                    const Scalar) const override {
        return {.t = infinity};
    };
};
/**
 * @brief represents camera interface in 3D
 */
using Camera3D = Camera<Dimension{3}>;

/**
 * @brief represents a pinhole camera
 */
template <Dimension DIMENSION>
class PinholeCamera : public Camera<DIMENSION> {
  public:
    /** @brief vector type */
    using VecType = Vec<DIMENSION>;
    /**
     * @brief representation of detector surface as a char
     * @see ray_for_coords
     */
    using DetectorSurface = std::function<VecType(const Scalar, const Scalar)>;

    /** @brief manifold for detector surface */
    DetectorSurface detector_surface;
    /** @brief position of pinhole */
    VecType pinhole;

    /**
     * @brief construct pinhole camera
     * @param pinhole pinhole position
     * @param surface_arg used to forward initialize detector_surface
     */
    template <class T>
    PinholeCamera(const VecType& pinhole, T surface_arg)
        : detector_surface(std::forward<T>(surface_arg)), pinhole(pinhole) {}

    std::unique_ptr<Ray<DIMENSION>>
    ray_for_coords(const Geometry<DIMENSION>& geometry, const Scalar x,
                   const Scalar y) const override {
        const VecType start = detector_surface(x, y);
        return geometry.ray_passing_through(start, pinhole);
    }
};

/**
 * @brief represents a pinhole camera in 3D
 */
using PinholeCamera3D = PinholeCamera<Dimension{3}>;

} // namespace cpp_raytracing

#endif
