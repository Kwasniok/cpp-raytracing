/**
 * @file
 * @brief animators for triangle entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_TRIANGLE_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_TRIANGLE_ANIMATOR_HPP

#include "../entities/triangle.hpp"
#include "base.hpp"

namespace cpp_raytracing {

class Entity;

/**
 * @brief animators update the current state of a triangle entity based on a
 *        given time
 */
class TriangleAnimator : public Animator {
  public:
    virtual ~TriangleAnimator() = default;

    /** @brief invoke update of entity for given time */
    virtual void update_for_time(const Scalar time, Entity* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time, Triangle* object) = 0;
};

void TriangleAnimator::update_for_time(const Scalar time, Entity* entity) {
    if (entity == nullptr)
        return;
    Triangle* tri = dynamic_cast<Triangle*>(entity);
    if (tri) {
        update_for_time_hook(time, tri);
    } else {
        throw_bad_entity_type("Triangle", entity->id);
    }
}

} // namespace cpp_raytracing

#endif
