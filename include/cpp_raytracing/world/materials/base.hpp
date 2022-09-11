/**
 * @file
 * @brief material interface
 */

#ifndef CPP_RAYTRACING_MATERIALS_BASE_HPP
#define CPP_RAYTRACING_MATERIALS_BASE_HPP

#include <utility>

#include "../../values/color.hpp"
#include "../../values/identifier.hpp"
#include "../../values/scalar.hpp"
#include "../hit_record.hpp"
#include "../ray.hpp"

namespace cpp_raytracing {

/**
 * @brief object material interface
 */
class Material {
  public:
    /** @brief unique material identifier */
    Identifier<class Material> id;

    /** @brief default construct with default idenfifier root */
    Material() = default;
    /** @brief move constructor */
    Material(Material&& other) = default;

    virtual ~Material() = default;

    /**
     * @brief calculates scattered ray and coloring based on the ray hitting the
     *        object
     * @note If the returned ray has a direction which is perfectly zero, the
     *       material is emissive at the scattering point.
     * @see Ray::direction_exactly_zero()
     */
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const = 0;
};

/** @brief default identifier for materials */
template <>
struct default_identifier<Material> {
    /** @brief default identifier for materials */
    static constexpr const char* value = "material";
};

} // namespace cpp_raytracing

#endif
