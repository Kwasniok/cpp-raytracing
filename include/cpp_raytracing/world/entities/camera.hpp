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
template <Dimension GEO_DIMENSION, Dimension PINHOLE_DIMENSION = GEO_DIMENSION>
requires(GEO_DIMENSION >= PINHOLE_DIMENSION) class PinholeCamera
    : public Camera<GEO_DIMENSION> {
  public:
    /**
     * @brief representation of detector surface as a chart of 2D coordinates
     *        with an additional time parameter
     * @see ray_for_coords
     */
    using DetectorSurface = std::function<Vec<GEO_DIMENSION>(
        const Scalar, const Scalar, const Scalar)>;

    /** @brief manifold for detector surface */
    DetectorSurface detector_surface;
    /**
     *@brief position of pinhole
     *@note The pinhole might be defined with respect to a submanifold. This
     *      allows for more flexibility of e.g. including or excluding a time
     *      dimension.
     */
    Vec<PINHOLE_DIMENSION> pinhole;
    /** @brief time */
    Scalar time = 0;

    /**
     * @brief construct pinhole camera
     * @param pinhole pinhole position
     * @param surface_arg used to forward initialize detector_surface
     */
    template <class T>
    PinholeCamera(const Vec<PINHOLE_DIMENSION>& pinhole, T surface_arg)
        : detector_surface(std::forward<T>(surface_arg)), pinhole(pinhole) {}

    /** @see Entity::set_time   */
    void set_time(const Scalar time) override {
        Camera<GEO_DIMENSION>::set_time(time);
    }

    std::unique_ptr<Ray<GEO_DIMENSION>>
    ray_for_coords(const Geometry<GEO_DIMENSION>& geometry, const Scalar x,
                   const Scalar y) const override {
        const Vec<GEO_DIMENSION> start = detector_surface(x, y, time);
        return geometry.ray_passing_through(start, pinhole);
    }
};

/**
 * @brief represents a pinhole camera in 3D
 */
using PinholeCamera3D = PinholeCamera<Dimension{3}>;

} // namespace cpp_raytracing

#endif
