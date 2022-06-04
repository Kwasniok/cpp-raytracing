/**
 * @file
 * @brief object
 */

#ifndef CPP_RAYTRACING_ENTITIES_OBJECT_HPP
#define CPP_RAYTRACING_ENTITIES_OBJECT_HPP

#include "../vec3.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

class Animator;

/**
 * @brief represents an object
 */
struct Object : public Entity {
  public:
    /** @brief object's current position in scene */
    Vec3 position = {0.0, 0.0, 0.0};
    /** @brief object's curren ratation in scene */
    Vec3 rotation = {0.0, 0.0, 0.0};
    /** @brief object's curren scale in scene */
    Vec3 scale = {1.0, 1.0, 1.0};

    /** @brief default construct with default idenfifier root */
    Object() = default;
    /** @brief move constructor */
    Object(Object&& other) = default;

    virtual ~Object() = default;
};

/** @brief default identifier for objects */
template <>
struct default_identifier<Object> {
    /** @brief default identifier for objects */
    static constexpr const char* value = "object";
};

} // namespace cpp_raytracing

#endif
