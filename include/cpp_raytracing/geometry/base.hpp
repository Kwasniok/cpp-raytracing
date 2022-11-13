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
template <Dimension DIMENSION>
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
    virtual std::optional<RaySegment<DIMENSION>> next_ray_segment() = 0;
};

/** @brief ray interface for 3D manifolds */
using Ray3D = Ray<Dimension{3}>;

/**
 * @brief geometry interface
 */
template <Dimension DIMENSION>
class Geometry {
  public:
    /** @brief vector type */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief Jacobian type for to ONB conversions */
    using ToONBJac = Mat<Dimension{3}, DIMENSION>;
    /** @brief Jacobian type for from ONB conversions */
    using FromONBJac = Mat<DIMENSION, Dimension{3}>;
    /** @brief metric type  */
    using Metric = Mat<Dimension{3}, Dimension{3}>;

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
    virtual std::unique_ptr<Ray<DIMENSION>>
    ray_from(const VolumeVec& start, const VolumeVec& direction) const = 0;

    /** @brief returns a ray with given starting point and target */
    virtual std::unique_ptr<Ray<DIMENSION>>
    ray_passing_through(const VolumeVec& start,
                        const VolumeVec& target) const = 0;

    /**
     * @brief returns the local Jacobian matrix converting vectors to
     *         canonical orthonormal representation
     * @param position curved space cooordinates
     * @see from_onb_jacobian
     */
    virtual ToONBJac to_onb_jacobian(const VolumeVec& position) const = 0;
    /**
     * @brief returns the local Jacobian matrix converting vectors back from the
     *        canonical orthonormal representation
     * @param position curved space cooordinates
     * @note Is the inverse matrix of `to_onb_jacobian(position)`.
     * @note to_onb_jacobian
     */
    virtual FromONBJac from_onb_jacobian(const VolumeVec& position) const = 0;

    /** @brief returns the local metric */
    virtual Metric metric(const VolumeVec& position) const = 0;
};

/** @brief geometry interface for 3D manifolds */
using Geometry3D = Geometry<Dimension{3}>;

} // namespace cpp_raytracing

#endif
