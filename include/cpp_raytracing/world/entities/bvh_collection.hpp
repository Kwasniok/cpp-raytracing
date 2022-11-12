/**
 * @file
 * @brief BVH collection
 */

#ifndef CPP_RAYTRACING_ENTITIES_BVH_COLLECTION_HPP
#define CPP_RAYTRACING_ENTITIES_BVH_COLLECTION_HPP

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "../../util.hpp"
#include "../../values/tensor.hpp"
#include "../bvh.hpp"
#include "base.hpp"
#include "camera.hpp"

namespace cpp_raytracing {

/**
 * @brief collection of entities with BVH optimization
 * @see BVHTree
 */
template <Dimension DIMENSION>
class BVHCollection : public Entity<DIMENSION> {
  public:
    /** @brief initialize with active camera */
    BVHCollection() = default;

    /** @brief copy constructor */
    BVHCollection(const BVHCollection&) = delete;

    /** @brief move constructor */
    BVHCollection(BVHCollection&&) = default;

    /** @brief copy assignment */
    BVHCollection& operator=(const BVHCollection&) = delete;

    /** @brief move assignment */
    BVHCollection& operator=(BVHCollection&&) = default;

    ~BVHCollection() override = default;

    /**
     * @brief remove all entities.
     * @note Not thread-safe.
     */
    inline void clear() {
        invalidate_cache();
        _entities.clear();
    }
    /**
     * @brief add an entity.
     * @note Not thread-safe.
     * @note Nested collections are not permitted.
     */
    inline void add(std::shared_ptr<Entity<DIMENSION>>&& entity) {
        if (is_instanceof<BVHCollection>(entity.get())) {
            throw std::runtime_error("Nested collections are not supported.");
        }
        invalidate_cache();
        _entities.push_back(std::move(entity));
    }

    /**
     * @note Invalidates cache.
     * @note Not thread-safe.
     */
    void set_time(const Scalar time) override;

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    HitRecord<DIMENSION> hit_record(const Geometry& geometry,
                                    const RaySegment<DIMENSION>& ray_segment,
                                    const Scalar t_min = 0.0) const override;

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override;

  public:
    /**
     * @brief invalidates the BVH cache
     * @note Not thread-safe.
     * @see BVHTree
     */
    void invalidate_cache() { _bvh_tree.reset(); }
    /**
     * @brief retursn true iff the BVH cache is generadted and up-to-date
     * @note Thread-safe.
     * @see BVHTree
     */
    bool cache_valid() const { return _bvh_tree.has_value(); }
    /**
     * @brief unconditionally generates the BVH cache
     * @note It MUST be called before any geometrical property of the collection
     *       can be querried. This includes hit_record and bounding_box.
     * @note Not thread-safe.
     * @see hit_record bounding_box
     */
    void generate_cache();
    /**
     * @brief conditionally generates the BVH cache if not valid
     * @note Not thread-safe.
     */
    void ensure_cache() {
        if (!cache_valid()) {
            generate_cache();
        }
    }

  private:
    std::vector<std::shared_ptr<Entity<DIMENSION>>> _entities;
    std::optional<BVHTree<DIMENSION>> _bvh_tree{std::nullopt};

  private:
    /** @brief like sourrounding_box but for optional values */
    static std::optional<AxisAlignedBoundingBox<DIMENSION>> surrounding_opt_box(
        const std::optional<AxisAlignedBoundingBox<DIMENSION>>& box1,
        const std::optional<AxisAlignedBoundingBox<DIMENSION>>& box2) {
        if (box1 && box2) {
            return surrounding_opt_box(*box1, *box2);
        }
        return std::nullopt;
    }
};

template <Dimension DIMENSION>
void BVHCollection<DIMENSION>::set_time(const Scalar time) {
    invalidate_cache();
    for (const auto& entity : _entities) {
        entity->set_time(time);
    }
}

template <Dimension DIMENSION>
HitRecord<DIMENSION>
BVHCollection<DIMENSION>::hit_record(const Geometry& geometry,
                                     const RaySegment<DIMENSION>& ray_segment,
                                     const Scalar t_min) const {
    if (_bvh_tree) {
        return _bvh_tree->hit_record(geometry, ray_segment, t_min);
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

template <Dimension DIMENSION>
void BVHCollection<DIMENSION>::generate_cache() {
    _bvh_tree = BVHTree<DIMENSION>(_entities);
}

template <Dimension DIMENSION>
std::optional<AxisAlignedBoundingBox<DIMENSION>>
BVHCollection<DIMENSION>::bounding_box() const {
    if (_bvh_tree) {
        return _bvh_tree->bounding_box();
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

/** @brief 3D BVH collection entity */
using BVHCollection3D = BVHCollection<Dimension{3}>;

} // namespace cpp_raytracing

#endif
