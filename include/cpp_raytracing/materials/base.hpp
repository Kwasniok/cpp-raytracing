/**
 * @file
 * @brief material interface
 */

#ifndef CPP_RAYTRACING_MATERIALS_BASE_HPP
#define CPP_RAYTRACING_MATERIALS_BASE_HPP

#include "../color.hpp"
#include "../hittables.hpp"
#include "../ray.hpp"
#include "../scalar.hpp"

namespace cpp_raytracing {

/**
 * @brief object material interface
 */
class Material {

  public:
    /**
     * @brief calculates scattered ray and coloring based on the ray hitting the
     * object
     */
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const = 0;

    virtual ~Material() = default;
};

} // namespace cpp_raytracing

#endif
