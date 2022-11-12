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
class TriangleAnimator3D : public Animator3D {
  public:
    /** @brief default constructor */
    TriangleAnimator3D() = default;

    /** @brief copy constructor */
    TriangleAnimator3D(const TriangleAnimator3D&) = default;

    /** @brief move constructor */
    TriangleAnimator3D(TriangleAnimator3D&&) = default;

    /** @brief copy assignment */
    TriangleAnimator3D& operator=(const TriangleAnimator3D&) = default;

    /** @brief move assignment */
    TriangleAnimator3D& operator=(TriangleAnimator3D&&) = default;

    ~TriangleAnimator3D() override = default;

    /** @brief invoke update of entity for given time */
    void update_for_time(const Scalar time, Entity3D* entity) override;

  protected:
    /** @brief hook for update_for_time */
    virtual void update_for_time_hook(const Scalar time,
                                      Triangle3D* object) = 0;
};

void TriangleAnimator3D::update_for_time(const Scalar time, Entity3D* entity) {
    if (entity == nullptr)
        return;
    Triangle3D* tri = dynamic_cast<Triangle3D*>(entity);
    if (tri) {
        update_for_time_hook(time, tri);
    } else {
        throw_bad_entity_type("Triangle", entity->id);
    }
}

} // namespace cpp_raytracing

#endif
