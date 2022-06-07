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

namespace internal {

std::optional<AxisAlignedBoundingBox>
update_super_box(const std::optional<AxisAlignedBoundingBox>& super,
                 const std::optional<AxisAlignedBoundingBox>& include) {
    if (super && include) {
        auto [x_min, x_max] = std::minmax(super->min()[0], include->min()[0]);
        auto [y_min, y_max] = std::minmax(super->min()[1], include->min()[1]);
        auto [z_min, z_max] = std::minmax(super->min()[2], include->min()[2]);

        return AxisAlignedBoundingBox{Vec3{x_min, y_min, z_min},
                                      Vec3{x_max, y_max, z_max}};
    }

    return std::nullopt;
}

} // namespace internal

std::optional<AxisAlignedBoundingBox> Scene::bounding_box() const {
    std::optional<AxisAlignedBoundingBox> super_boundaries{std::nullopt};

    bool is_first = true;
    for (const auto& entity : _entities) {
        if (is_first) {
            super_boundaries = entity->bounding_box();
            is_first = false;
        } else {
            internal::update_super_box(super_boundaries,
                                       entity->bounding_box());
        }
        if (!super_boundaries.has_value()) {
            break;
        }
    }

    return super_boundaries;
}

} // namespace cpp_raytracing

#endif
