/**
 * @file
 * @brief hittable and transformable object
 */

#ifndef CPP_RAYTRACING_HITTABLES_TRANSFORMABLE_HPP
#define CPP_RAYTRACING_HITTABLES_TRANSFORMABLE_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief hittable and tranformable object
 */
class Transformable : public Hittable {
  public:
    /** @brief current position */
    Vec3 position;
    /**
     * @brief current velocity
     * @note `position + velocity = next(position)position of next frame`
     */
    Vec3 velocity;
    virtual ~Transformable() = default;

    /**
     * @brief effective subframe position
     * @param subframe_time 0.0..1.0
     * @note Required for motion blur.
     */
    Vec3 subframe_position(const Scalar subframe_time) const {
        return position + subframe_time * velocity;
    }
};

} // namespace cpp_raytracing

#endif
