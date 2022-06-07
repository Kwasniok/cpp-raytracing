/**
 * @file
 * @brief entity
 */

#ifndef CPP_RAYTRACING_ENTITIES_ENTITY_HPP
#define CPP_RAYTRACING_ENTITIES_ENTITY_HPP

#include <memory>
#include <optional>

#include "../animators/animator.hpp"
#include "../bounding_volume.hpp"
#include "../hit_record.hpp"
#include "../identifier.hpp"
#include "../scalar.hpp"

namespace cpp_raytracing {

/**
 * @brief represents an entity
 */
struct Entity {
    /** @brief unique entity identifier */
    Identifier<class Entity> id;

    /** @brief default construct with default idenfifier root */
    Entity() = default;
    /** @brief move constructor */
    Entity(Entity&& other) = default;
    /** @brief move assignment */
    Entity& operator=(Entity&& other) = default;

    virtual ~Entity() = default;

    /**
     * @brief sets the animator
     * @note none removes the animator and means the entity is static
     */
    virtual void set_animator(std::unique_ptr<Animator>&& animator) {
        _animator = std::move(animator);
    }
    /** @brief returns the animator of the entity */
    virtual Animator& animator() { return *_animator; }

    /**
     * @brief requests entity to take its appearence for the given time
     * @note A static entity might ignore this entirely.
     */
    virtual void set_time(const Scalar time) {
        if (_animator) {
            _animator->update_for_time(time, this);
        }
    }

    /**
     * @brief calculates the intersection of the light ray with the object
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * ::infinity
     */
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const {
        return {.t = infinity};
    }

    /**
     * @brief might return a boundaring box if the entity is bounded
     * @param time_min start of the time interval
     * @param time_max end of the time interval
     */
    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const {
        return std::nullopt;
    }

  private:
    /**
     * @brief animator of the entity (optional)
     * @note none means the entity is static.
     */
    std::unique_ptr<Animator> _animator;
};

/** @brief default identifier for entitys */
template <>
struct default_identifier<Entity> {
    /** @brief default identifier for entitys */
    static constexpr const char* value = "entity";
};

} // namespace cpp_raytracing

#endif
