/**
 * @file
 * @brief bounding volume
 */

#ifndef CPP_RAYTRACING_BOUNDING_VOLUME_HPP
#define CPP_RAYTRACING_BOUNDING_VOLUME_HPP

#include <algorithm>
#include <cmath>

#include "ray.hpp"
#include "util.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief represents a bounding volume
 * @see BVHTree
 */
class AxisAlignedBoundingBox {

  public:
    /** @brief construct box from two arbitrary corners */
    constexpr AxisAlignedBoundingBox(const Vec3 corner1, const Vec3 corner2) {
        const auto [x_min, x_max] = minmax(corner1[0], corner2[0]);
        const auto [y_min, y_max] = minmax(corner1[1], corner2[1]);
        const auto [z_min, z_max] = minmax(corner1[2], corner2[2]);
        _min = {x_min, y_min, z_min};
        _max = {x_max, y_max, z_max};
    }

    /** @brief corner of the ox with smallest coefficients */
    constexpr Vec3 min() const { return _min; }
    /** @brief corner of the ox with largest coefficients */
    constexpr Vec3 max() const { return _max; }

    /** @brief tests if ray hits any section of the volume within the box */
    constexpr bool hit(const Ray ray, Scalar t_min, Scalar t_max) const {

        // note: mind branch prediction and reduce
        for (int i = 0; i < 3; ++i) {

            if (ray.direction()[i] == 0.0) [[unlikely]] {
                if (ray.start()[i] < _min[i] || ray.start()[i] > _max[i]) {
                    return false;
                }
            } else [[likely]] {

                const Scalar d = 1.0 / ray.direction()[i];
                const Scalar ta = (_min[i] - ray.start()[i]) * d;
                const Scalar tb = (_max[i] - ray.start()[i]) * d;
                if (std::isnan(ta) || std::isnan(tb)) {
                    return false;
                }

                const auto [t0, t1] = minmax(ta, tb);

                t_min = std::max(t_min, t0);
                t_max = std::min(t_max, t1);

                if (t_max <= t_min) {
                    return false;
                }
            }
        }

        return true;
    }

  private:
    friend constexpr AxisAlignedBoundingBox
    surrounding_box(const AxisAlignedBoundingBox& box1,
                    const AxisAlignedBoundingBox& box2);

    /** @brief dummy type to indicate internal trusted calls */
    struct Trusted {};

    /**
     * @brief internal only: construct box from min and max corners
     * @param min MUST be corner with minimal coefficients
     * @param max MUST be corner with maximal coefficients
     */
    constexpr AxisAlignedBoundingBox(const Vec3 min, const Vec3 max, Trusted)
        : _min(min), _max(max) {}

  private:
    Vec3 _min;
    Vec3 _max;
};

/** @brief construct smallest box containing both boxes */
constexpr AxisAlignedBoundingBox
surrounding_box(const AxisAlignedBoundingBox& box1,
                const AxisAlignedBoundingBox& box2) {
    const Vec3 min1 = box1.min();
    const Vec3 min2 = box2.min();
    const auto x_min = std::min(min1[0], min2[0]);
    const auto y_min = std::min(min1[1], min2[1]);
    const auto z_min = std::min(min1[2], min2[2]);
    const Vec3 max1 = box1.max();
    const Vec3 max2 = box2.max();
    const auto x_max = std::max(max1[0], max2[0]);
    const auto y_max = std::max(max1[1], max2[1]);
    const auto z_max = std::max(max1[2], max2[2]);
    return AxisAlignedBoundingBox{Vec3{x_min, y_min, z_min},
                                  Vec3{x_max, y_max, z_max},
                                  AxisAlignedBoundingBox::Trusted{}};
}

} // namespace cpp_raytracing

#endif
