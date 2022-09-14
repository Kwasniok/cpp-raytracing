/**
 * @file
 * @brief module for hit records
 */

// NOTE: This has its own file to reduce dependencies.

#ifndef CPP_RAYTRACING_HIT_RECORD_HPP
#define CPP_RAYTRACING_HIT_RECORD_HPP

#include <memory>

#include "../values/scalar.hpp"
#include "../values/tensor.hpp"
#include "ray.hpp"

namespace cpp_raytracing {

class Material;

/**
 * @brief records all information of a ray hitting (part of) an object
 * @note mediates between hittables and materials
 */
struct HitRecord {
    /** @brief local metric for #point */
    Mat3x3 metric;
    /** @brief intersection point of ray and object */
    Vec3 point;
    /** @brief surface normal (**points outwards**) */
    Vec3 normal;
    /** @brief texture coordinates */
    Vec2 uv_coordinates;
    /** @brief material of the object */
    const Material* material;
    /** @brief RaySegment parameter of #point */
    Scalar t;
    /** @brief ray his surface from the outside */
    bool front_face;

    /**
     * @brief sets normal and front_face
     */
    void set_face_normal(const RaySegment& ray, const Vec3& face_normal) {
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
