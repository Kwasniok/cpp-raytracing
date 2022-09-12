/**
 * @file
 * @brief portal material
 */

#ifndef CPP_RAYTRACING_MATERIALS_PORTAL_HPP
#define CPP_RAYTRACING_MATERIALS_PORTAL_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief portal material (transformation of rays passing through)
 */
class Portal : public Material {
  public:
    /** @brief small scalar value */
    constexpr static Scalar epsilon = 1.0e-8;

    /** @brief translation */
    Vec3 position = {0.0, 0.0, 0.0};
    /**
     * @brief rotation (Trait-Bryan angles)
     * @note The application order is: roll, pitch, yaw
     * @see rotation_mat
     */
    Vec3 rotation = {0.0, 0.0, 0.0};
    /** @brief scaling */
    Vec3 scale = {1.0, 1.0, 1.0};

    virtual ~Portal() = default;

    virtual std::pair<RaySegment, Color> scatter(const HitRecord& record,
                                          const RaySegment& ray) const override {

        const auto transformation = rotation_mat(rotation) * scaling_mat(scale);

        const Vec3 point = transformation * record.point + position;
        const Vec3 direction = transformation * ray.direction();

        // position outgoing ray slightly outside
        return {RaySegment(point + epsilon * direction, direction), Colors::WHITE};
    }
};

} // namespace cpp_raytracing

#endif
