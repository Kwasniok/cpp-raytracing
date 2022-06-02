/**
 * @file
 * @brief Hittable base module
 */

#ifndef CPP_RAYTRACING_HITTABLES_BASE_HPP
#define CPP_RAYTRACING_HITTABLES_BASE_HPP

#include <memory>

#include "../hit_record.hpp"
#include "../identifier.hpp"
#include "../ray.hpp"
#include "../scalar.hpp"
#include "../vec3.hpp"

namespace cpp_raytracing {

/**
 * @brief Interface of objects which interact with light rays.
 */
class Hittable {
  public:
    /** @brief unique hittable identifier */
    Identifier<class Hittable> id;

    /** @brief default construct with default idenfifier root */
    Hittable() = default;
    /** @brief move constructor */
    Hittable(Hittable&& other) = default;

    /**
     * @brief calculates the intersection of the light ray with the object
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * ::infinity
     */
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const = 0;

    virtual ~Hittable() = default;
};

/** @brief default identifier for hittables */
template <>
struct default_identifier<Hittable> {
    /** @brief default identifier for hittables */
    static constexpr const char* value = "hittable";
};

} // namespace cpp_raytracing

#endif
