/**
 * @file
 * @brief hittable triangular object
 */

#ifndef CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP
#define CPP_RAYTRACING_ENTITIES_TRIANGLE_HPP

#include <utility>

#include "../../values/tensor.hpp"
#include "../materials/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief triangular object
 * @note The face is filled via bilinear coordinate interpolation.
 * @note Triangle exists mostly for debugging and reference purposes. Consider
 *       using Mesh before using Triangle.
 */
template <Dimension DIMENSION>
class Triangle : public Entity<DIMENSION> {
  public:
    /** @brief position vector type */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief face corners */
    std::array<VolumeVec, 3> points;
    /** @brief material of the plane */
    std::shared_ptr<Material<DIMENSION>> material;

    /** @brief default constructor */
    Triangle() = default;

    /** @brief copy constructor */
    Triangle(const Triangle&) = delete;

    /** @brief move constructor */
    Triangle(Triangle&&) = default;

    /** @brief copy assignment */
    Triangle& operator=(const Triangle&) = delete;

    /** @brief move assignment */
    Triangle& operator=(Triangle&&) = default;

    ~Triangle() override = default;

    /** @see Entity::bounding_box */
    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override;
};

template <Dimension DIMENSION>
std::optional<AxisAlignedBoundingBox<DIMENSION>>
Triangle<DIMENSION>::bounding_box() const {

    constexpr Scalar epsilon = 1e-8;

    Vec<DIMENSION> low = {+infinity, +infinity, +infinity};
    Vec<DIMENSION> high = {-infinity, -infinity, -infinity};

    for (const auto& point : points) {
        low.inplace_elementwise(min, point);
        high.inplace_elementwise(max, point);
    }

    // padding to guarantee non-zero volume
    low -= low.elementwise(abs) * epsilon;
    high += high.elementwise(abs) * epsilon;

    return AxisAlignedBoundingBox<DIMENSION>{low, high};
}

/**
 * @brief triangle object in a 3D maifold
 * @see Triangle
 */
using Triangle3D = Triangle<Dimension{3}>;

} // namespace cpp_raytracing

#endif
