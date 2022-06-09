/**
 * @file
 * @brief scene representation
 */

#ifndef CPP_RAYTRACING_ENTITIES_SCENE_HPP
#define CPP_RAYTRACING_ENTITIES_SCENE_HPP

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "bvh.hpp"
#include "entities/bvh_collection.hpp"
#include "entities/camera.hpp"
#include "entities/entity.hpp"
#include "util.hpp"

namespace cpp_raytracing {

/**
 * @brief Collection of entities and a camera.
 */
class Scene {
  public:
    /** @brief initialize with active camera */
    Scene() = default;
    /** @brief move constructor */
    Scene(Scene&&) = default;

    /**
     * @brief remove all entities.
     * @note Not thread-safe.
     */
    inline void clear() { _collection.clear(); }
    /**
     * @brief add an entity.
     * @note Not thread-safe.
     * @note Nested scenes are not permitted.
     */
    inline void add(std::unique_ptr<Entity>&& entity) {
        _collection.add(std::move(entity));
    }

    /**
     * @note Invalidates cache.
     * @note Not thread-safe.
     */
    void set_time(const Scalar time) { _collection.set_time(time); }

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                         const Scalar t_max = infinity) const {
        return _collection.hit_record(ray, t_min, t_max);
    }

    /**
     * @note Requires valid cache.
     * @note Thread-safe.
     */
    std::optional<AxisAlignedBoundingBox> bounding_box() const {
        return _collection.bounding_box();
    }

  public:
    /**
     * @brief invalidates the BVH cache
     * @note Not thread-safe.
     * @see BVHTree
     */
    void invalidate_cache() { _collection.invalidate_cache(); }
    /**
     * @brief retursn true iff the BVH cache is generadted and up-to-date
     * @note Thread-safe.
     * @see BVHTree
     */
    bool cache_valid() const { return _collection.cache_valid(); }
    /**
     * @brief unconditionally generates the BVH cache
     * @note It MUST be called before any geometrical property of the scene
     *       can be querried. This includes hit_record and bounding_box.
     * @note Not thread-safe.
     * @see hit_record bounding_box
     */
    void generate_cache() { _collection.generate_cache(); }
    /**
     * @brief conditionally generates the BVH cache if not valid
     * @note Not thread-safe.
     */
    void ensure_cache() { _collection.ensure_cache(); }

  public:
    /** @brief active camera of the scene used for rendering */
    std::unique_ptr<Camera> active_camera;

  private:
    BVHCollection _collection;
};

} // namespace cpp_raytracing

#endif
