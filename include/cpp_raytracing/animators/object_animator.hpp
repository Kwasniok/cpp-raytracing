/**
 * @file
 * @brief object animator
 */

#ifndef CPP_RAYTRACING_ANIMATORS_OBJECT_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_OBJECT_ANIMATOR_HPP

#include <memory>

#include "../entities/object.hpp"
#include "../math.hpp"
#include "animator.hpp"

namespace cpp_raytracing {

/**
 * @brief basic object animator
 */
class ObjectAnimator : public Animator {
  public:
    virtual ~ObjectAnimator() = default;

    /** @brief invoke update of entity for given time */
    virtual void update_for_time(const Scalar time, Entity* entity) override;

  protected:
    /** @brief hook for update_for_time*/
    virtual void update_for_time_hook(const Scalar time, Object* object) = 0;
};

void ObjectAnimator::update_for_time(const Scalar time, Entity* entity) {
    if (entity == nullptr)
        return;
    Object* object = dynamic_cast<Object*>(entity);
    if (object) {
        update_for_time_hook(time, object);
    } else {
        throw_bad_entity_type("Object", entity->id);
    }
}

} // namespace cpp_raytracing

#endif
