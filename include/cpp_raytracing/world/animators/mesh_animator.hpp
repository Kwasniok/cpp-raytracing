/**
 * @file
 * @brief animators for mesh entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_MESH_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_MESH_ANIMATOR_HPP

#include "../entities/mesh.hpp"
#include "base.hpp"

namespace cpp_raytracing {

class Entity;

/**
 * @brief animators update the current state of a mesh entity based on a
 *        given time
 */
class MeshAnimator : public Animator {
  public:
    virtual ~MeshAnimator() = default;

    virtual void update_for_time(const Scalar time, Entity* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time, Mesh* mesh) = 0;
};

void MeshAnimator::update_for_time(const Scalar time, Entity* entity) {
    if (entity == nullptr)
        return;
    Mesh* mesh = dynamic_cast<Mesh*>(entity);
    if (mesh) {
        update_for_time_hook(time, mesh);
    } else {
        throw_bad_entity_type("Mesh", mesh->id);
    }
}

} // namespace cpp_raytracing

#endif
