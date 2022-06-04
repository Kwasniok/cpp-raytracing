/**
 * @file
 * @brief animator
 */

#ifndef CPP_RAYTRACING_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATOR_HPP

#include "scalar.hpp"

namespace cpp_raytracing {

class Entity;

/**
 * @brief animaotrs update the current state of an entity based on a given time
 */
class Animator {
  public:
    virtual ~Animator() = default;

    /** @brief invoke update of entity for given time */
    virtual void update_for_time(const Scalar time, Entity* entity) = 0;
};

} // namespace cpp_raytracing

#endif
