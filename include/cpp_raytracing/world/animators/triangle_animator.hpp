/**
 * @file
 * @brief animators for triangle entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_TRIANGLE_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_TRIANGLE_ANIMATOR_HPP

#include "../entities/triangle.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief animators update the current state of a triangle entity based on a
 *        given time
 */
template <Dimension DIMENSION>
// clang-tidy false-positive: https://github.com/llvm/llvm-project/issues/59032
// NOLINTNEXTLINE(cppcoreguidelines-virtual-class-destructor)
class TriangleAnimator : public Animator<DIMENSION> {
  public:
    /** @brief default constructor */
    TriangleAnimator() = default;

    /** @brief copy constructor */
    TriangleAnimator(const TriangleAnimator&) = default;

    /** @brief move constructor */
    TriangleAnimator(TriangleAnimator&&) = default;

    /** @brief copy assignment */
    TriangleAnimator& operator=(const TriangleAnimator&) = default;

    /** @brief move assignment */
    TriangleAnimator& operator=(TriangleAnimator&&) = default;

    ~TriangleAnimator() override = default;

    /** @brief invoke update of entity for given time */
    void update_for_time(const Scalar time, Entity<DIMENSION>* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time,
                                      Triangle<DIMENSION>* object) = 0;
};

template <Dimension DIMENSION>
void TriangleAnimator<DIMENSION>::update_for_time(const Scalar time,
                                                  Entity<DIMENSION>* entity) {
    if (entity == nullptr)
        return;
    Triangle<DIMENSION>* tri = dynamic_cast<Triangle<DIMENSION>*>(entity);
    if (tri) {
        update_for_time_hook(time, tri);
    } else {
        this->throw_bad_entity_type("Triangle", entity->id);
    }
}

/**
 * @brief animator for triangle entities in a 3D maifold
 * @see TriangleAnimator
 */
using TriangleAnimator3D = TriangleAnimator<Dimension{3}>;

} // namespace cpp_raytracing

#endif
