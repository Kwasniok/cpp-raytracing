/**
 * @file
 * @brief hittable (small) triangular object in 3D maifolds
 */

#ifndef CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_3D
#define CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_3D

#include "small_triangle_util.hpp"
#include "triangle.hpp"

namespace cpp_raytracing {

/**
 * @brief (small) triangle in a 3D manifold
 * @note The face is filled via bilinear coordinate interpolation.
 * @note 'small' means there is no significant deviation from flat space across
 *       each individual triangle. This means coordinate deltas and local
 *       tangential vectors are approxiately proportional within each
 *       individual triangle.
 * @note SmallTriangle3D exists mostly for debugging and reference purposes.
 *       Consider using Mesh before using SmallTriangleMesh3D.
 */
class SmallTriangle3D : public Triangle3D {
  public:
    /** @brief default constructor */
    SmallTriangle3D() = default;

    /** @brief copy constructor */
    SmallTriangle3D(const SmallTriangle3D&) = delete;

    /** @brief move constructor */
    SmallTriangle3D(SmallTriangle3D&&) = default;

    /** @brief copy assignment */
    SmallTriangle3D& operator=(const SmallTriangle3D&) = delete;

    /** @brief move assignment */
    SmallTriangle3D& operator=(SmallTriangle3D&&) = default;

    ~SmallTriangle3D() override = default;

    /** @see Entity::hit_record
     * @note Works for 'small' triangles only - i.e. there MUST NOT be any
     *       significant curvature across each triangle of the mesh.
     */
    HitRecord3D hit_record(const Geometry3D& geometry,
                           const RaySegment3D& ray_segment,
                           const Scalar t_min = 0.0) const override;
};

HitRecord3D SmallTriangle3D::hit_record(const Geometry3D& geometry,
                                        const RaySegment3D& ray_segment,
                                        const Scalar t_min) const {
    using namespace tensor;
    return small_triangle_hit_record_3d(geometry, ray_segment, t_min,
                                        this->points[0], this->points[1],
                                        this->points[2], this->material.get());
}

} // namespace cpp_raytracing

#endif
