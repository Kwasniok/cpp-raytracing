/**
 * @file
 * @brief object
 */

#ifndef CPP_RAYTRACING_OBJECT_HPP
#define CPP_RAYTRACING_OBJECT_HPP

#include "identifier.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief represents an object
 */
struct Object {
    /** @brief unique object identifier */
    Identifier<class Object> id;
    /** @brief object position in scene */
    Vec3 position = {0.0, 0.0, 0.0};
    /** @brief object ratation in scene */
    Vec3 rotation = {0.0, 0.0, 0.0};
    /** @brief object scale in scene */
    Vec3 scale = {1.0, 1.0, 1.0};

    /** @brief default construct with default idenfifier root */
    Object() = default;
    /** @brief move constructor */
    Object(Object&& other) = default;
};

/** @brief default identifier for objects */
template <>
struct default_identifier<Object> {
    /** @brief default identifier for objects */
    static constexpr const char* value = "object";
};

} // namespace cpp_raytracing

#endif
