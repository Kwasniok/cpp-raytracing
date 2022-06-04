/**
 * @file
 * @brief scene representation
 */

#ifndef CPP_RAYTRACING_ENTITIES_SCENE_HPP
#define CPP_RAYTRACING_ENTITIES_SCENE_HPP

#include <memory>
#include <vector>

#include "../camera.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief Collection of entities and a camera.
 */
class Scene : public Entity {
  public:
    /** @brief initialize with active camera */
    Scene(const Camera& camera) : camera(camera){};
    /** @brief move constructor */
    Scene(Scene&&) = default;
    virtual ~Scene() = default;

    /** @brief remove all entities. */
    inline void clear() { _entities.clear(); }
    /** @brief add an entity. */
    inline void add(std::unique_ptr<Entity>&& entity) {
        _entities.push_back(std::move(entity));
    }

    virtual void set_time(const Scalar time) override;

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

  public:
    /** @brief active camera of the scene used for rendering */
    Camera camera;

  private:
    std::vector<std::unique_ptr<Entity>> _entities;
};

void Scene::set_time(const Scalar time) {
    for (const auto& entity : _entities) {
        entity->set_time(time);
    }
}

HitRecord Scene::hit_record(const Ray& ray, const Scalar t_min,
                            const Scalar t_max) const {
    HitRecord closest_record = {.t = infinity};

    for (const auto& entity : _entities) {
        HitRecord record = entity->hit_record(ray, t_min, t_max);
        if (record.t < closest_record.t) {
            closest_record = record;
        }
    }

    return closest_record;
}

} // namespace cpp_raytracing

#endif
