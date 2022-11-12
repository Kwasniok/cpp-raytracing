/**
 * @file
 * @brief bounding volume
 */

#ifndef CPP_RAYTRACING_BOUNDING_VOLUME_HPP
#define CPP_RAYTRACING_BOUNDING_VOLUME_HPP

#include <algorithm>
#include <cmath>

#include "../util.hpp"
#include "../values/tensor.hpp"
#include "ray_segment.hpp"

namespace cpp_raytracing {

template <Dimension DIMENSION>
class AxisAlignedBoundingBox;

template <Dimension DIMENSION>
constexpr AxisAlignedBoundingBox<DIMENSION>
surrounding_box(const AxisAlignedBoundingBox<DIMENSION>& box1,
                const AxisAlignedBoundingBox<DIMENSION>& box2);

template <Dimension DIMENSION>
constexpr AxisAlignedBoundingBox<DIMENSION>
operator+(const AxisAlignedBoundingBox<DIMENSION>& box,
          const Vec<DIMENSION>& vec);

/**
 * @brief represents a bounding volume
 * @see BVHTree
 */
template <Dimension DIMENSION>
class AxisAlignedBoundingBox {

  public:
    /** @brief vector type */
    using VecType = Vec<DIMENSION>;

    /** @brief construct box from two arbitrary corners */
    constexpr AxisAlignedBoundingBox(const VecType corner1,
                                     const VecType corner2) {
        for (Dimension i = {0}; i < DIMENSION; ++i) {
            const auto [low, high] = minmax(corner1[i], corner2[i]);
            _min[i] = low;
            _max[i] = high;
        }
    }

    /** @brief corner of the ox with smallest coefficients */
    constexpr VecType min() const { return _min; }
    /** @brief corner of the ox with largest coefficients */
    constexpr VecType max() const { return _max; }

    /** @brief tests if ray hits any section of the volume within the box */
    constexpr bool hit(const RaySegment<DIMENSION> ray, Scalar t_min,
                       Scalar t_max) const {

        // note: mind branch prediction and reduce
        for (unsigned int i = 0; i < DIMENSION; ++i) {

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

    /** @brief shifts the box by the given vector */
    constexpr AxisAlignedBoundingBox& operator+=(const VecType& vec) {
        _min += vec;
        _max += vec;
        return *this;
    }

  private:
    friend constexpr AxisAlignedBoundingBox
    surrounding_box<>(const AxisAlignedBoundingBox& box1,
                      const AxisAlignedBoundingBox& box2);
    // clang-format off
    friend constexpr AxisAlignedBoundingBox
    operator+<>(const AxisAlignedBoundingBox& box, const VecType& vec);
    // clang-format on

    /** @brief dummy type to indicate internal trusted calls */
    struct Trusted {};

    /**
     * @brief internal only: construct box from min and max corners
     * @param min MUST be corner with minimal coefficients
     * @param max MUST be corner with maximal coefficients
     */
    constexpr AxisAlignedBoundingBox(const VecType min, const VecType max,
                                     Trusted)
        : _min(min), _max(max) {}

  private:
    VecType _min;
    VecType _max;
};

/** @brief construct smallest box containing both boxes */
template <Dimension DIMENSION>
constexpr AxisAlignedBoundingBox<DIMENSION>
surrounding_box(const AxisAlignedBoundingBox<DIMENSION>& box1,
                const AxisAlignedBoundingBox<DIMENSION>& box2) {
    return {
        box1.min().elementwise(min, box2.min()),
        box1.max().elementwise(max, box2.max()),
        typename AxisAlignedBoundingBox<DIMENSION>::Trusted{},
    };
}

/** @brief shifts the box by the given vector */
template <Dimension DIMENSION>
constexpr AxisAlignedBoundingBox<DIMENSION>
operator+(const AxisAlignedBoundingBox<DIMENSION>& box,
          const Vec<DIMENSION>& vec) {
    return {
        box.min() + vec,
        box.max() + vec,
        typename AxisAlignedBoundingBox<DIMENSION>::Trusted{},
    };
}

/** @brief 3D axis aligned bounding box */
using AxisAlignedBoundingBox3D = AxisAlignedBoundingBox<Dimension{3}>;

} // namespace cpp_raytracing

#endif
