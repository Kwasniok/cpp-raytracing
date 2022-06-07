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

} // namespace cpp_raytracing

#endif
