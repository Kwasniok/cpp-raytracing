/**
 * @file
 * @brief linear motion animators for mesh entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_LINEAR_MOTION_MESH_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_LINEAR_MOTION_MESH_ANIMATOR_HPP

#include <cpp_raytracing/world/animators/mesh_animator.hpp>

namespace cpp_raytracing {

/**
 * @brief linear motion based mesh entity animator
 */
template <Dimension DIMENSION>
class LinearMotionMeshAnimator : public MeshAnimator<DIMENSION> {
  public:
    /** @brief manifold vector */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief start position for `time = time_offset` of all points */
    std::vector<VolumeVec> start_points;
    /** @brief constant velocity */
    VolumeVec velocity;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    /** @brief default constructor */
    LinearMotionMeshAnimator() = default;

    /** @brief copy constructor */
    LinearMotionMeshAnimator(const LinearMotionMeshAnimator&) = delete;

    /** @brief move constructor */
    LinearMotionMeshAnimator(LinearMotionMeshAnimator&&) = default;

    /** @brief copy assignment */
    LinearMotionMeshAnimator&
    operator=(const LinearMotionMeshAnimator&) = delete;

    /** @brief move assignment */
    LinearMotionMeshAnimator& operator=(LinearMotionMeshAnimator&&) = default;

    ~LinearMotionMeshAnimator() override = default;

  protected:
    /** @see MeshAnimator::update_for_time_hook */
    void update_for_time_hook(const Scalar time,
                              Mesh<DIMENSION>* mesh) override;
};

template <Dimension DIMENSION>
void LinearMotionMeshAnimator<DIMENSION>::update_for_time_hook(
    const Scalar time, Mesh<DIMENSION>* mesh) {
    if (mesh == nullptr)
        return;
    mesh->points = start_points;
    for (auto& point : mesh->points) {
        point = point + velocity * (time - time_offset);
    }
}

/** @brief linear motion mesh animator for 3D manifolds */
using LinearMotionMeshAnimator3D = LinearMotionMeshAnimator<Dimension{3}>;

} // namespace cpp_raytracing

#endif
