/**
 * @file
 * @brief geometry interface
 */

#ifndef CPP_RAYTRACING_GEOMETRY_BASE_HPP
#define CPP_RAYTRACING_GEOMETRY_BASE_HPP

#include <memory>

#include "../values/tensor.hpp"
#include "../world/ray_segment.hpp"

namespace cpp_raytracing {

/**
 * @brief ray interface
 * @note A ray is approximated by straight ray segments.
 */
class Ray {
  public:
    /** @brief default constructor */
    Ray() = default;

    /** @brief copy constructor */
    Ray(const Ray&) = default;

    /** @brief move constructor */
    Ray(Ray&&) = default;

    /** @brief copy assignment */
    Ray& operator=(const Ray&) = default;

    /** @brief move assignment */
    Ray& operator=(Ray&&) = default;

    virtual ~Ray() = default;

    /**
     * @brief returns next ray segment or nothing
     * @note A ray may end prematurely due to technical limitations.
     */
    virtual std::optional<RaySegment3D> next_ray_segment() = 0;
};

/**
 * @brief geometry interface
 */
class Geometry {
  public:
    /** @brief default constructor */
    Geometry() = default;

    /** @brief copy constructor */
    Geometry(const Geometry&) = default;

    /** @brief move constructor */
    Geometry(Geometry&&) = default;

    /** @brief copy assignment */
    Geometry& operator=(const Geometry&) = default;

    /** @brief move assignment */
    Geometry& operator=(Geometry&&) = default;

    virtual ~Geometry() = default;

    /**
     * @brief returns a ray with given starting point and direction
     * @param start origin of ray
     * @param direction normalized direction tangential vector
     */
    virtual std::unique_ptr<Ray> ray_from(const Vec3& start,
                                          const Vec3& direction) const = 0;

    /** @brief returns a ray with given starting point and target */
    virtual std::unique_ptr<Ray>
    ray_passing_through(const Vec3& start, const Vec3& target) const = 0;

    /**
     * @brief returns the local Jacobian matrix converting vectors to
     *         canonical orthonormal representation
     * @param position curved space cooordinates
     * @see from_onb_jacobian
     */
    virtual Mat3x3 to_onb_jacobian(const Vec3& position) const = 0;
    /**
     * @brief returns the local Jacobian matrix converting vectors back from the
     *        canonical orthonormal representation
     * @param position curved space cooordinates
     * @note Is the inverse matrix of `to_onb_jacobian(position)`.
     * @note to_onb_jacobian
     */
    virtual Mat3x3 from_onb_jacobian(const Vec3& position) const = 0;

    /** @brief returns the local metric */
    virtual Mat3x3 metric(const Vec3& position) const = 0;
};

} // namespace cpp_raytracing

#endif
