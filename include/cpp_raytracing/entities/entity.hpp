/**
 * @file
 * @brief entity
 */

#ifndef CPP_RAYTRACING_ENTITIES_ENTITY_HPP
#define CPP_RAYTRACING_ENTITIES_ENTITY_HPP

#include "../animator.hpp"
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
    /**
     * @brief animator of the entity (optional)
     * @note `nullptr` means the entity is static.
     */
    Animator* animator;

    /** @brief default construct with default idenfifier root */
    Entity() = default;
    /** @brief move constructor */
    Entity(Entity&& other) = default;

    virtual ~Entity() = default;

    /**
     * @brief requests entity to take its appearence for the given time
     * @note A static entity might ignore this entirely.
     */
    virtual void set_time(const Scalar time) {
        if (animator != nullptr) {
            animator->update_for_time(time, this);
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
};

/** @brief default identifier for entitys */
template <>
struct default_identifier<Entity> {
    /** @brief default identifier for entitys */
    static constexpr const char* value = "entity";
};

} // namespace cpp_raytracing

#endif
