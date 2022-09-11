/**
 * @file
 * @brief background interface
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_BASE_HPP
#define CPP_RAYTRACING_BACKGROUNDS_BASE_HPP

#include "../../values/color.hpp"
#include "../../values/identifier.hpp"

namespace cpp_raytracing {

/**
 * @brief background interface
 */
class Background {
  public:
    /** @brief unique background identifier */
    Identifier<class Background> id;

    /** @brief default construct with default idenfifier root */
    Background() = default;
    /** @brief move constructor */
    Background(Background&& other) = default;

    virtual ~Background() = default;

    /**
     * @brief color value for background direction
     * @param direction non-zero vector
     */
    virtual Color value(const Vec3& direction) const = 0;

    /** @brief default background */
    constexpr static Color value_for_default_background(const Vec3& direction) {
        constexpr Color light{1.0, 1.0, 1.0};
        constexpr Color dark{0.5, 0.7, 1.0};

        const auto dir = unit_vector(direction);
        const auto t = 0.5 * (std::abs(dir.y()) + 1.0);
        return (1.0 - t) * light + t * dark;
    }
};

/** @brief default identifier for backgrounds */
template <>
struct default_identifier<Background> {
    /** @brief default identifier for backgrounds */
    static constexpr const char* value = "background";
};

} // namespace cpp_raytracing

#endif
