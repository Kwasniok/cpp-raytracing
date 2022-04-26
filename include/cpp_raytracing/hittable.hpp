/**
 * @file
 * @brief Hittable interface
 */

#ifndef CPP_RAYTRACING_HITTABLE_HPP
#define CPP_RAYTRACING_HITTABLE_HPP

#include <memory>

#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

class Material;

/**
 * @brief records all information of a ray hitting (part of) an object
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
};

/**
 * @brief Interface of objects which interact with light rays.
 */
class Hittable {
  public:
    /**
     * @brief calculates the intersection of the light ray with the object
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * ::SCALAR_INF
     */
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = SCALAR_INF) const = 0;

    virtual ~Hittable() = default;
};

} // namespace ray

#endif
