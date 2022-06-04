/**
 * @file
 * @brief Hittable base module
 */

#ifndef CPP_RAYTRACING_HITTABLES_BASE_HPP
#define CPP_RAYTRACING_HITTABLES_BASE_HPP

#include "../hit_record.hpp"
#include "../ray.hpp"
#include "../scalar.hpp"
namespace cpp_raytracing {

/**
 * @brief Interface of objects which interact with light rays.
 * @note interface only
 */
class Hittable {
  public:
    /**
     * @brief calculates the intersection of the light ray with the object
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * ::infinity
     */
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const = 0;

    /**
     * @brief requests hittable to take its appearence for the given time
     * @note A static hittable might ignore this entirely.
     * @note Setting the time is part of the Hittable interface to enable
     * effects like motion blur and rolling shutter. Both require to slightly
     * change the time of the scene during the rendering process.
     */
    virtual void set_time(const Scalar time) = 0;

    virtual ~Hittable() = default;
};

} // namespace cpp_raytracing

#endif
