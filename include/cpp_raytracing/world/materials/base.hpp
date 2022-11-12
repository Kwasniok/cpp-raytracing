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
#include "../../values/tensor.hpp"
#include "../hit_record.hpp"

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

    /** @brief copy constructor */
    Material(const Material&) = delete;

    /** @brief move constructor */
    Material(Material&&) = default;

    /** @brief copy assignment */
    Material& operator=(const Material&) = delete;

    /** @brief move assignment */
    Material& operator=(Material&&) = default;

    virtual ~Material() = default;

    /**
     * @brief calculates direction of scattered ray and coloring based on the
     *        ray hitting the surface
     * @note If the returned direction is perfectly zero, the material is
     *       emissive.
     * @note All vectors are asserted to be relative to an othronormal basis!
     *       (The tangential space of a surface is flat.)
     * @see zero_vec<3_D>
     */
    virtual std::pair<Vec3, Color> scatter(const HitRecord3D& record,
                                           const Vec3& ray_direction) const = 0;
};

/** @brief default identifier for materials */
template <>
struct default_identifier<Material> {
    /** @brief default identifier for materials */
    static constexpr const char* value = "material";
};

} // namespace cpp_raytracing

#endif
