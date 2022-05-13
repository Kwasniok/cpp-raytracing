/**
 * @file
 * @brief material interface
 */

#ifndef CPP_RAYTRACING_MATERIALS_BASE_HPP
#define CPP_RAYTRACING_MATERIALS_BASE_HPP

#include <utility>

#include "../color.hpp"
#include "../hittables.hpp"
#include "../identifier.hpp"
#include "../ray.hpp"
#include "../scalar.hpp"

namespace cpp_raytracing {

/**
 * @brief object material interface
 */
class Material {
  public:
    /** @brief initialize with a unique default identifier */
    Material() = default;

    /** @brief initialize with identifier */
    Material(Identifier<class Material>&& id) : id(std::move(id)) {}
    /**
     * @brief initialize with (similar) identifier
     * @see Identifier::make_always
     */
    Material(const char* id) {
        this->id = Identifier<class Material>::make_always({id});
    }
    /**
     * @brief initialize with (similar) identifier
     * @see Identifier::make_always
     */
    Material(const std::string& id) {
        this->id = Identifier<class Material>::make_always(std::string{id});
    }
    /**
     * @brief initialize with (similar) identifier
     * @see Identifier::make_always
     */
    Material(std::string&& id) {
        this->id = Identifier<class Material>::make_always(std::move(id));
    }

    /** @brief moves identifier */
    Material(Material&&) = default;
    /** @brief moves identifier */
    Material& operator=(Material&&) = default;
    /**
     * @brief non-copiable due to identifier
     * @note use copy() instead
     */
    Material(const Material&) = delete;
    /**
     * @brief non-copiable due to identifier
     * @note use copy() instead
     */
    Material& operator=(const Material&) = delete;

    /**
     * @brief calculates scattered ray and coloring based on the ray hitting the
     * object
     */
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const = 0;

    virtual ~Material() = default;

  public:
    /** @brief unique material identifier */
    Identifier<class Material> id;
};

/** @brief default identifier for materials */
template <>
struct default_identifier<Material> {
    /** @brief default identifier for materials */
    static constexpr const char* value = "material";
};

} // namespace cpp_raytracing

#endif
