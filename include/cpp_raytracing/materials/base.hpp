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
    Material() = default;
    Material(Material&&) = default;
    Material& operator=(Material&&) = default;
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    Material(Identifier<class Material>&& id) : id(std::move(id)) {}
    Material(const char* id) {
        this->id = Identifier<class Material>::make_always({id});
    }
    Material(const std::string& id) {
        this->id = Identifier<class Material>::make_always(std::string{id});
    }
    Material(std::string&& id) {
        this->id = Identifier<class Material>::make_always(std::move(id));
    }

    /**
     * @brief calculates scattered ray and coloring based on the ray hitting the
     * object
     */
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const = 0;

    virtual ~Material() = default;

  public:
    Identifier<class Material> id;
};

template <>
struct default_identifier<Material> {
    static constexpr const char* value = "material";
};

} // namespace cpp_raytracing

#endif
