/**
 * @file
 * @brief sinosoidal motion animators for mesh entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_SINOSOIDAL_MOTION_MESH_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_SINOSOIDAL_MOTION_MESH_ANIMATOR_HPP

#include <cpp_raytracing/world/animators/mesh_animator.hpp>

namespace cpp_raytracing {

/**
 * @brief sinusoidal motion based mesh entity animator
 */
template <Dimension DIMENSION>
class SinusoidalMotionMeshAnimator : public MeshAnimator<DIMENSION> {
  public:
    /** @brief manifold vector */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief start position for `time = time_offset` of all points */
    std::vector<VolumeVec> start_points;
    /** @brief oscillator amplitude */
    VolumeVec amplitude;
    /** @brief frequency of motion in radians */
    Scalar frequency = 0.0;
    /** @brief time for which `position = start` */
    Scalar time_offset = 0.0;

    /** @brief default constructor */
    SinusoidalMotionMeshAnimator() = default;

    /** @brief copy constructor */
    SinusoidalMotionMeshAnimator(const SinusoidalMotionMeshAnimator&) = delete;

    /** @brief move constructor */
    SinusoidalMotionMeshAnimator(SinusoidalMotionMeshAnimator&&) = default;

    /** @brief copy assignment */
    SinusoidalMotionMeshAnimator&
    operator=(const SinusoidalMotionMeshAnimator&) = delete;

    /** @brief move assignment */
    SinusoidalMotionMeshAnimator&
    operator=(SinusoidalMotionMeshAnimator&&) = default;

    ~SinusoidalMotionMeshAnimator() override = default;

  protected:
    /** @see MeshAnimator::update_for_time_hook */
    void update_for_time_hook(const Scalar time,
                              Mesh<DIMENSION>* mesh) override;
};

template <Dimension DIMENSION>
void SinusoidalMotionMeshAnimator<DIMENSION>::update_for_time_hook(
    const Scalar time, Mesh<DIMENSION>* mesh) {
    if (mesh == nullptr)
        return;

    mesh->points = start_points;
    for (auto& point : mesh->points) {
        point = point + std::sin(frequency * (time - time_offset)) * amplitude;
    }
}

/** @brief sinosoidal motion mesh animator for 3D manifolds */
using SinusoidalMotionMeshAnimator3D =
    SinusoidalMotionMeshAnimator<Dimension{3}>;

} // namespace cpp_raytracing

#endif
