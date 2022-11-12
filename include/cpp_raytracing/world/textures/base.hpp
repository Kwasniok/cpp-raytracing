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
template <Dimension DIMENSION>
class Texture {
  public:
    /** @brief volume vector type */
    using VolumeVec = Vec<DIMENSION>;

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
    virtual Color value(const Vec2& uv_coordinates,
                        const VolumeVec& point) const = 0;

    /** @brief indicates a missing texture */
    constexpr static Color
    value_for_missing_texture([[maybe_unused]] const Vec2& uv_coordinates,
                              [[maybe_unused]] const VolumeVec& point) {
        constexpr Scalar scale = 1 / 10.0;
        constexpr Color light{1.0, 0.0, 1.0};
        constexpr Color dark{0.1, 0.0, 0.1};

        const int val =
            int(uv_coordinates[0] / scale) + int(uv_coordinates[1] / scale);
        const bool is_light = val % 2 == 0;

        return is_light ? light : dark;
    }
};

/** @brief texture of 3D entity */
using Texture3D = Texture<Dimension{3}>;

/** @brief default identifier for textures */
template <Dimension DIMENSION>
struct default_identifier<Texture<DIMENSION>> {
    /** @brief default identifier for textures */
    static constexpr const char* value = "texture";
};

} // namespace cpp_raytracing

#endif
