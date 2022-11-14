/**
 * @file
 * @brief animators for mesh entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_MESH_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_MESH_ANIMATOR_HPP

#include "../entities/mesh.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief animators update the current state of a mesh entity based on a
 *        given time
 */
template <Dimension DIMENSION>
class MeshAnimator : public Animator<DIMENSION> {
  public:
    /** @brief default constructor */
    MeshAnimator() = default;

    /** @brief copy constructor */
    MeshAnimator(const MeshAnimator&) = default;

    /** @brief move constructor */
    MeshAnimator(MeshAnimator&&) = default;

    /** @brief copy assignment */
    MeshAnimator& operator=(const MeshAnimator&) = default;

    /** @brief move assignment */
    MeshAnimator& operator=(MeshAnimator&&) = default;

    ~MeshAnimator() override = default;

    /** @see Entity::update_for_time */
    void update_for_time(const Scalar time, Entity<DIMENSION>* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time,
                                      Mesh<DIMENSION>* mesh) = 0;
};

template <Dimension DIMENSION>
void MeshAnimator::update_for_time(const Scalar time,
                                   Entity<DIMENSION>* entity) {
    if (entity == nullptr)
        return;
    Mesh<DIMENSION>* mesh = dynamic_cast<Mesh<DIMENSION>*>(entity);
    if (mesh) {
        update_for_time_hook(time, mesh);
    } else {
        throw_bad_entity_type("Mesh", mesh->id);
    }
}

/**
 * @brief animator for mesh entities in a 3D maifold
 * @see MeshAnimator
 */
using MeshAnimator3D = MeshAnimator<Dimension{3}>;

} // namespace cpp_raytracing

#endif
