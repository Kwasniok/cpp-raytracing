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
    /** @brief current velocity */
    Vec3 velocity;
    virtual ~Transformable() = default;

};

} // namespace cpp_raytracing

#endif
