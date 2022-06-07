/**
 * @file
 * @brief bounding volume (BVH)
 */

#ifndef CPP_RAYTRACING_BOUNDING_VOLUME_HPP
#define CPP_RAYTRACING_BOUNDING_VOLUME_HPP

#include <algorithm>

#include "ray.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief represents a bounding volume (BVH)
 */
class AxisAlignedBoundingBox {
  public:
    /** @brief construct box with 'lower left' and 'upper right' cornder */
    AxisAlignedBoundingBox(const Vec3 min, const Vec3 max)
        : _min(min), _max(max) {}

    /** @brief corner of the ox with smallest coefficients */
    Vec3 min() const { return _min; }
    /** @brief corner of the ox with largest coefficients */
    Vec3 max() const { return _max; }

    /** @brief tests if ray hits any section of the volume within the box */
    bool hit(const Ray ray, const Scalar t_min, const Scalar t_max) const {
        // note: mind branch prediction and reduce
        for (int i = 0; i < 3; ++i) {
            const Scalar ta = (_min[i] - ray.start()[i]) / ray.direction()[i];
            const Scalar tb = (_max[i] - ray.start()[i]) / ray.direction()[i];

            const auto [t0, t1] = std::minmax(ta, tb);

            if (t0 < t_min || t1 > t_max) {
                return false;
            }
        }
        return true;
    }

  private:
    Vec3 _min;
    Vec3 _max;
};

/** @brief construct smallest box containing both boxes */
inline AxisAlignedBoundingBox
surrounding_box(const AxisAlignedBoundingBox& box1,
                const AxisAlignedBoundingBox& box2) {
    auto [x_min, x_max] = std::minmax(box1.min()[0], box2.min()[0]);
    auto [y_min, y_max] = std::minmax(box1.min()[1], box2.min()[1]);
    auto [z_min, z_max] = std::minmax(box1.min()[2], box2.min()[2]);

    return AxisAlignedBoundingBox{Vec3{x_min, y_min, z_min},
                                  Vec3{x_max, y_max, z_max}};
}

} // namespace cpp_raytracing

#endif
