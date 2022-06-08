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

#include "../bvh.hpp"
#include "../util.hpp"
#include "camera.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief Collection of entities and a camera.
 */
class Scene : public Entity {
  public:
    /** @brief initialize with active camera */
    Scene() = default;
    /** @brief move constructor */
    Scene(Scene&&) = default;
    virtual ~Scene() = default;

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
     * @note Nested scenes are not permitted.
     */
    inline void add(std::unique_ptr<Entity>&& entity) {
        if (is_instanceof<Scene>(entity.get())) {
            throw std::runtime_error("Nested scenes are not supported.");
        }
        invalidate_cache();
        _entities.push_back(std::move(entity));
    }

    /**
     * @note Invalidates cache.
     * @note Not thread-safe.
     */
    void set_time(const Scalar time);

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
     * @note It MUST be called before any geometrical property of the scene
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

  public:
    /** @brief active camera of the scene used for rendering */
    std::unique_ptr<Camera> active_camera;

  private:
    std::vector<std::unique_ptr<Entity>> _entities;
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

void Scene::set_time(const Scalar time) {
    invalidate_cache();
    for (const auto& entity : _entities) {
        entity->set_time(time);
    }
}

HitRecord Scene::hit_record(const Ray& ray, const Scalar t_min,
                            const Scalar t_max) const {
    if (_bvh_tree) {
        return _bvh_tree->hit_record(ray, t_min, t_max);
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

void Scene::generate_cache() {
    _bvh_tree = BVHTree(_entities);
}

std::optional<AxisAlignedBoundingBox> Scene::bounding_box() const {
    if (_bvh_tree) {
        return _bvh_tree->bounding_box();
    } else {
        throw std::runtime_error(
            "Missing BVH cache. Call generate_cache() first.");
    }
}

} // namespace cpp_raytracing

#endif
