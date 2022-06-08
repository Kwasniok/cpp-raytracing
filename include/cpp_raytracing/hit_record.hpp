/**
 * @file
 * @brief module for hit records
 */

// NOTE: This has its own file to reduce dependencies.

#ifndef CPP_RAYTRACING_HIT_RECORD_HPP
#define CPP_RAYTRACING_HIT_RECORD_HPP

#include <memory>

#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace cpp_raytracing {

class Material;

/**
 * @brief records all information of a ray hitting (part of) an object
 * @note mediates between hittables and materials
 */
struct HitRecord {
    /** @brief intersection point of ray and object */
    Vec3 point;
    /** @brief surface normal (**points outwards**) */
    Vec3 normal;
    /** @brief material of the object */
    std::shared_ptr<Material> material;
    /** @brief Ray parameter of #point */
    Scalar t;
    /** @brief ray his surface from the outside */
    bool front_face;

    /**
     * @brief sets normal and front_face
     */
    void set_face_normal(const Ray& ray, const Vec3& face_normal) {
        front_face = dot(face_normal, ray.direction()) < 0.0;
        normal = front_face ? face_normal : -face_normal;
    }

    /**
     * @brief returns true iff t is finite
     * @note Used for debugging and testing.
     */
    bool hits() const { return t < infinity; }
};

} // namespace cpp_raytracing

#endif
