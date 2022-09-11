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
#include "../bvh.hpp"
#include "camera.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief collection of entities with BVH optimization
 * @see BVHTree
 */
class BVHCollection : public Entity {
  public:
    /** @brief initialize with active camera */
    BVHCollection() = default;
    /** @brief move constructor */
    BVHCollection(BVHCollection&&) = default;
    virtual ~BVHCollection() = default;

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
    inline void add(std::shared_ptr<Entity>&& entity) {
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
    virtual void set_time(const Scalar time) override;

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

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
    std::vector<std::shared_ptr<Entity>> _entities;
    std::optional<BVHTree> _bvh_tree{std::nullopt};

  private:
    /** @brief like sourrounding_box but for optional values */
    static std::optional<AxisAlignedBoundingBox>
    surrounding_opt_box(const std::optional<AxisAlignedBoundingBox>& box1,
                        const std::optional<AxisAlignedBoundingBox>& box2) {
        if (box1 && box2) {
            return surrounding_opt_box(*box1, *box2);
        }
        return std::nullopt;
    }
};

void BVHCollection::set_time(const Scalar time) {
    invalidate_cache();
    for (const auto& entity : _entities) {
        entity->set_time(time);
    }
}

HitRecord BVHCollection::hit_record(const Ray& ray, const Scalar t_min,
                                    const Scalar t_max) const {
    if (_bvh_tree) {
        return _bvh_tree->hit_record(ray, t_min, t_max);
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

void BVHCollection::generate_cache() {
    _bvh_tree = BVHTree(_entities);
}

std::optional<AxisAlignedBoundingBox> BVHCollection::bounding_box() const {
    if (_bvh_tree) {
        return _bvh_tree->bounding_box();
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

} // namespace cpp_raytracing

#endif
