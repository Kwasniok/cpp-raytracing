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
class MeshAnimator3D : public Animator3D {
  public:
    /** @brief default constructor */
    MeshAnimator3D() = default;

    /** @brief copy constructor */
    MeshAnimator3D(const MeshAnimator3D&) = default;

    /** @brief move constructor */
    MeshAnimator3D(MeshAnimator3D&&) = default;

    /** @brief copy assignment */
    MeshAnimator3D& operator=(const MeshAnimator3D&) = default;

    /** @brief move assignment */
    MeshAnimator3D& operator=(MeshAnimator3D&&) = default;

    ~MeshAnimator3D() override = default;

    void update_for_time(const Scalar time, Entity3D* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time, Mesh3D* mesh) = 0;
};

void MeshAnimator3D::update_for_time(const Scalar time, Entity3D* entity) {
    if (entity == nullptr)
        return;
    Mesh3D* mesh = dynamic_cast<Mesh3D*>(entity);
    if (mesh) {
        update_for_time_hook(time, mesh);
    } else {
        throw_bad_entity_type("Mesh", mesh->id);
    }
}

} // namespace cpp_raytracing

#endif
