/**
 * @file
 * @brief hittable mesh object consisting  of (small) triangles in a 3D manifold
 */

#ifndef CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_MESH_HPP
#define CPP_RAYTRACING_ENTITIES_SMALL_TRIANGLE_MESH_HPP

#include "mesh.hpp"
#include "small_triangle_util.hpp"

namespace cpp_raytracing {

/**
 * @brief mesh object of (small) triangles in a 3D manifold
 * @note Each face is filled via bilinear coordinate interpolation.
 * @note 'small' means there is no significant deviation from flat space across
 *       each individual triangle. This means coordinate deltas and local
 *       tangential vectors are approxiately proportional within each
 *       individual triangle.
 */
class SmallTriangleMesh3D : public Mesh3D {
  public:
    /** @brief default constructor */
    SmallTriangleMesh3D() = default;

    /** @brief copy constructor */
    SmallTriangleMesh3D(const SmallTriangleMesh3D&) = delete;

    /** @brief move constructor */
    SmallTriangleMesh3D(SmallTriangleMesh3D&&) = default;

    /** @brief copy assignment */
    SmallTriangleMesh3D& operator=(const SmallTriangleMesh3D&) = delete;

    /** @brief move assignment */
    SmallTriangleMesh3D& operator=(SmallTriangleMesh3D&&) = default;

    ~SmallTriangleMesh3D() override = default;

    /**
     * @see Entity::hit_record
     * @note Works for 'small' triangles only - i.e. there MUST NOT be any
     *       significant curvature across each triangle of the mesh.
     */
    HitRecord3D hit_record(const Geometry3D& geometry,
                           const RaySegment3D& ray_segment,
                           const Scalar t_min = 0.0) const override;
};

HitRecord3D SmallTriangleMesh3D::hit_record(const Geometry3D& geometry,
                                            const RaySegment3D& ray_segment,
                                            const Scalar t_min) const {
    HitRecord3D record = {.t = infinity};
    for (const auto& face : this->faces) {
        HitRecord3D rec = small_triangle_hit_record_3d(
            geometry, ray_segment, t_min, this->points[face[0]],
            this->points[face[1]], this->points[face[2]], this->material.get());
        if (rec.t < record.t) {
            record = rec;
        }
    }
    return record;
}

} // namespace cpp_raytracing

#endif
