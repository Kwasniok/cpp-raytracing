/**
 * @file
 * @brief scene representation
 */

#ifndef CPP_RAYTRACING_ENTITIES_SCENE_HPP
#define CPP_RAYTRACING_ENTITIES_SCENE_HPP

#include <algorithm>
#include <memory>
#include <vector>

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

    /** @brief remove all entities. */
    inline void clear() { _entities.clear(); }
    /** @brief add an entity. */
    inline void add(std::unique_ptr<Entity>&& entity) {
        _entities.push_back(std::move(entity));
    }

    virtual void set_time(const Scalar time) override;

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

  public:
    /** @brief active camera of the scene used for rendering */
    std::unique_ptr<Camera> active_camera;

  private:
    std::vector<std::unique_ptr<Entity>> _entities;

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

std::optional<AxisAlignedBoundingBox> Scene::bounding_box() const {
    std::optional<AxisAlignedBoundingBox> super_boundaries{std::nullopt};

    bool is_first = true;
    for (const auto& entity : _entities) {
        if (is_first) {
            super_boundaries = entity->bounding_box();
            is_first = false;
        } else {
            super_boundaries =
                surrounding_opt_box(super_boundaries, entity->bounding_box());
        }
        if (!super_boundaries.has_value()) {
            break;
        }
    }

    return super_boundaries;
}

} // namespace cpp_raytracing

#endif
