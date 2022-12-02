/**
 * @file
 * @brief background interface
 */

#ifndef CPP_RAYTRACING_BACKGROUNDS_BASE_HPP
#define CPP_RAYTRACING_BACKGROUNDS_BASE_HPP

#include "../../values/color.hpp"
#include "../../values/identifier.hpp"
#include "../../values/tensor.hpp"
#include "../../world/geometries/base.hpp"
#include "../ray_segment.hpp"

namespace cpp_raytracing {

/**
 * @brief background interface
 */
template <Dimension DIMENSION>
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
    virtual Color value(const Geometry<DIMENSION>& geometry,
                        const RaySegment<DIMENSION>& ray_segment) const = 0;
};

/** @brief background interface for 3D manifolds */
using Background3D = Background<Dimension{3}>;

/** @brief default identifier for backgrounds */
template <Dimension DIMENSION>
struct default_identifier<Background<DIMENSION>> {
    /** @brief default identifier for backgrounds */
    static constexpr const char* value = "background";
};

} // namespace cpp_raytracing

#endif
