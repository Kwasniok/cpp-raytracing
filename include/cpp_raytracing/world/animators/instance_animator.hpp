/**
 * @file
 * @brief instance animator
 */

#ifndef CPP_RAYTRACING_ANIMATORS_INSTANCE_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_INSTANCE_ANIMATOR_HPP

#include <memory>

#include "../../values/tensor.hpp"
#include "../entities/instance.hpp"
#include "animator.hpp"

namespace cpp_raytracing {

/**
 * @brief basic instance animator
 */
class InstanceAnimator : public Animator {
  public:
    virtual ~InstanceAnimator() = default;

    /** @brief invoke update of entity for given time */
    virtual void update_for_time(const Scalar time, Entity* entity) override;

  protected:
    /** @brief hook for update_for_time*/
    virtual void update_for_time_hook(const Scalar time,
                                      Instance* instance) = 0;
};

void InstanceAnimator::update_for_time(const Scalar time, Entity* entity) {
    if (entity == nullptr)
        return;
    Instance* instance = dynamic_cast<Instance*>(entity);
    if (instance) {
        update_for_time_hook(time, instance);
    } else {
        throw_bad_entity_type("Instance", entity->id);
    }
}

} // namespace cpp_raytracing

#endif
