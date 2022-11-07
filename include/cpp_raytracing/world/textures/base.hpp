/**
 * @file
 * @brief texture interface
 */

#ifndef CPP_RAYTRACING_TEXTURES_BASE_HPP
#define CPP_RAYTRACING_TEXTURES_BASE_HPP

#include "../../values/color.hpp"
#include "../../values/identifier.hpp"
#include "../../values/tensor.hpp"

namespace cpp_raytracing {

/**
 * @brief texture interface
 */
class Texture {
  public:
    /** @brief unique texture identifier */
    Identifier<class Texture> id;

    /** @brief default construct with default identifier root */
    Texture() = default;

    /** @brief copy constructor */
    Texture(const Texture& other) = delete;

    /** @brief move constructor */
    Texture(Texture&& other) = default;

    /** @brief copy assignment */
    Texture& operator=(const Texture& other) = delete;

    /** @brief move assignment */
    Texture& operator=(Texture&& other) = default;

    virtual ~Texture() = default;

    /** @brief color value for UV coordinates and position in space */
    virtual Color value(const Vec2& coordinates, const Vec3& point) const = 0;

    /** @brief indicates a missing texture */
    constexpr static Color
    value_for_missing_texture([[maybe_unused]] const Vec2& coordinates,
                              [[maybe_unused]] const Vec3& point) {
        constexpr Scalar scale = 1 / 10.0;
        constexpr Color light{1.0, 0.0, 1.0};
        constexpr Color dark{0.1, 0.0, 0.1};

        const int val =
            int(coordinates[0] / scale) + int(coordinates[1] / scale);
        const bool is_light = val % 2 == 0;

        return is_light ? light : dark;
    }
};

/** @brief default identifier for textures */
template <>
struct default_identifier<Texture> {
    /** @brief default identifier for textures */
    static constexpr const char* value = "texture";
};

} // namespace cpp_raytracing

#endif
