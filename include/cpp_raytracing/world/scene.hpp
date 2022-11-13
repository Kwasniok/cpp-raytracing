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

#include "../util.hpp"
#include "backgrounds/base.hpp"
#include "bvh.hpp"
#include "entities/base.hpp"
#include "entities/bvh_collection.hpp"
#include "entities/camera.hpp"

namespace cpp_raytracing {

/**
 * @brief collection of entities and a camera.
 */
class Scene {

  public:
    /**
     * @brief freezes a Scene temporarily and provides a read-only interface to
     *        it
     * @note This class exists as a guard to lock a scene, build its cache and
     *       provide a read-only interface of the scene during rendering. The
     *       read-only mode makes the scene thread-safe.
     * @note Has full access to its related scene (is friend class of Scene).
     */
    class FreezeGuard {
      private:
        friend Scene;
        /**
         * @brief freezes a scene and build its cache for the given time
         * @note Since a scene might not have a valid active camera, an explicit
         *       reference is required.
         */
        FreezeGuard(Scene& scene, const Camera3D& active_camera,
                    const Scalar time);

      public:
        /** @brief copy constructor */
        FreezeGuard(const FreezeGuard&) = delete;

        /** @brief move constructor */
        FreezeGuard(FreezeGuard&&) = delete;

        /** @brief copy assignment */
        FreezeGuard& operator=(const FreezeGuard&) = delete;

        /** @brief move assignment */
        FreezeGuard& operator=(FreezeGuard&&) = delete;

        /** @brief unfreezes the scene */
        inline ~FreezeGuard();

        /**
         * @brief calculates the intersection of the light ray with the entities
         *        of the scene
         * @see Entity::hit_record
         */
        inline HitRecord3D hit_record(const Geometry3D& geometry,
                                      const RaySegment3D& ray_segment,
                                      const Scalar t_min = 0.0) const;

      public:
        /** @brief active camera of the frozen scene */
        const Camera3D& active_camera;
        /**
         * @brief (optional) active background of the frozen scene
         * @note Backgrounds do not interact with hit_record().
         */
        const Background3D* active_background;

      private:
        /** @brief frozen scene */
        Scene& _scene;
    };

  public:
    /**
     * @brief active camera of the scene used for rendering
     * @note An active camera is required for rendering a scene.
     */
    std::shared_ptr<Camera3D> active_camera;
    /**
     * @brief active background of the scene used for rendering
     * @note An active background is optional but recommended for rendering a
     *       scene.
     */
    std::shared_ptr<Background3D> active_background;

    /** @brief initialize with an active camera */
    Scene(std::shared_ptr<Camera3D> active_camera)
        : active_camera(active_camera){};

    /** @brief copy constructor */
    Scene(const Scene&) = delete;

    /** @brief move constructor */
    Scene(Scene&&) = default;

    /** @brief copy assignment */
    Scene& operator=(const Scene&) = delete;

    /** @brief move assignment */
    Scene& operator=(Scene&&) = default;

    ~Scene() = default;

    /**
     * @brief remove all entities
     * @note Not thread-safe.
     */
    inline void clear() {
        if (is_frozen()) {
            throw std::runtime_error("Cannot clear scene while frozen.");
        }
        _collection.clear();
    }
    /**
     * @brief add an entity
     * @note Not thread-safe.
     * @note Nested scenes are not permitted.
     */
    inline void add(std::shared_ptr<Entity3D>&& entity) {
        if (is_frozen()) {
            throw std::runtime_error(
                "Cannot add entity to scene while frozen.");
        }
        _collection.add(std::move(entity));
    }

    /**
     * @brief freezes the scene at a given time and returns a read-only
     *        interface to the scene
     * @note The entire scene is updated to the requested time, the scene is
     *       frozen and a cache is generated. While frozen it can no longer be
     *       modified. Once the lifetime of the guard ends, it is automatically
     *       unfrozen.
     * @see is_frozen
     */
    inline const FreezeGuard freeze_for_time(const Scalar time);

    /**
     * @brief returns true iff scene is frozen
     * @see freeze_for_time, FreezeGuard
     */
    inline bool is_frozen() { return _frozen; }

  private:
    /** @brief collection of entites with BVH optimization */
    BVHCollection3D _collection;
    /**
     * @brief true iff scene is frozen
     * @see freeze_for_time, FreezeGuard
     */
    bool _frozen = false;
};

inline Scene::FreezeGuard::FreezeGuard(Scene& scene,
                                       const Camera3D& active_camera,
                                       const Scalar time)
    : active_camera(active_camera), _scene(scene) {
    active_background = _scene.active_background.get();
    scene.active_camera->set_time(time);
    scene._collection.set_time(time);
    scene._collection.ensure_cache();
    scene._frozen = true;
}

inline Scene::FreezeGuard::~FreezeGuard() {
    _scene._frozen = false;
}

inline HitRecord3D
Scene::FreezeGuard::hit_record(const Geometry3D& geometry,
                               const RaySegment3D& ray_segment,
                               const Scalar t_min) const {
    return _scene._collection.hit_record(geometry, ray_segment, t_min);
}

inline const Scene::FreezeGuard Scene::freeze_for_time(const Scalar time) {
    if (is_frozen()) {
        throw std::runtime_error(
            "Cannot freeze scene more than once concurrently.");
    }
    const Camera3D* const camera = active_camera.get();
    if (camera == nullptr) {
        throw std::runtime_error(
            "Cannot freeze scene without an active camera.");
    }
    return FreezeGuard(*this, *camera, time);
}

} // namespace cpp_raytracing

#endif
