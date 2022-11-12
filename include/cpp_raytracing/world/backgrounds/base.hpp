/**
 * @file
 * @brief background interface
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_BASE_HPP
#define CPP_RAYTRACING_BACKGROUNDS_BASE_HPP

#include "../../geometry/base.hpp"
#include "../../values/color.hpp"
#include "../../values/identifier.hpp"
#include "../ray_segment.hpp"

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

    /** @brief copy constructor */
    Background(const Background&) = delete;

    /** @brief move constructor */
    Background(Background&&) = default;

    /** @brief copy assignment */
    Background& operator=(const Background&) = delete;

    /** @brief move assignment */
    Background& operator=(Background&&) = default;

    virtual ~Background() = default;

    /**
     * @brief color value for background from ray_segment
     * @param geometry linked geometry
     * @param ray_segment last segment of ray which was simulated
     */
    virtual Color value(const Geometry& geometry,
                        const RaySegment3D& ray_segment) const = 0;
};

/** @brief default identifier for backgrounds */
template <>
struct default_identifier<Background> {
    /** @brief default identifier for backgrounds */
    static constexpr const char* value = "background";
};

} // namespace cpp_raytracing

#endif
