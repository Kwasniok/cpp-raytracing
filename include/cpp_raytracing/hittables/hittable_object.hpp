/**
 * @file
 * @brief hittable object
 */

#ifndef CPP_RAYTRACING_HITTABLES_HITTABLE_OBJECT_HPP
#define CPP_RAYTRACING_HITTABLES_HITTABLE_OBJECT_HPP

#include "../object.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief hittable object
 */
class HittableObject : public Hittable, public Object {};

} // namespace cpp_raytracing

#endif
