/**
 * @file
 * @brief hittable mesh object
 */

#ifndef CPP_RAYTRACING_ENTITIES_MESH_HPP
#define CPP_RAYTRACING_ENTITIES_MESH_HPP

#include <array>
#include <memory>
#include <vector>

#include "../../values/tensor.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief mesh object
 * @note Each face is filled via bilinear coordinate interpolation.
 */
template <Dimension DIMENSION>
class Mesh : public Entity<DIMENSION> {
  public:
    /** @brief position vector type */
    using VolumeVec = Vec<DIMENSION>;
    /** @brief point index type */
    using Index = std::vector<VolumeVec>::size_type;
    /** @brief face type (triplet of point indices)*/
    using Face = std::array<Index, 3>;

    /** @brief face corners */
    std::vector<VolumeVec> points;
    /** @brief faces */
    std::vector<Face> faces;
    /** @brief material of the plane */
    std::shared_ptr<Material> material;

    /** @brief default constructor */
    Mesh() = default;

    /** @brief copy constructor */
    Mesh(const Mesh&) = delete;

    /** @brief move constructor */
    Mesh(Mesh&&) = default;

    /** @brief copy assignment */
    Mesh& operator=(const Mesh&) = delete;

    /** @brief move assignment */
    Mesh& operator=(Mesh&&) = default;

    ~Mesh() override = default;

    /** @see Entity::set_time */
    void set_time(const Scalar time) override {
        Entity<DIMENSION>::set_time(time);
        generate_cache();
    }

    /** @see Entity::bounding_box */
    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override {
        return {_bounds};
    }

  private:
    void generate_cache() {

        constexpr Scalar epsilon = 1e-8;

        VolumeVec low = {+infinity, +infinity, +infinity};
        VolumeVec high = {-infinity, -infinity, -infinity};

        for (const auto& point : points) {
            low.inplace_elementwise(min, point);
            high.inplace_elementwise(max, point);
        }

        // padding to guarantee non-zero volume
        low -= low.elementwise(abs) * epsilon;
        high += high.elementwise(abs) * epsilon;

        for (unsigned int i = 0; i < 3; ++i) {
            if (low[i] == high[i]) {
                low[i] -= epsilon;
                high[i] += epsilon;
            }
        }

        _bounds = {low, high};
    }

  private:
    // note: initial value is irrelevant
    AxisAlignedBoundingBox<DIMENSION> _bounds = {
        tensor::zero_vec<DIMENSION>,
        tensor::zero_vec<DIMENSION>,
    };
};

/**
 * @brief represents a mesh based on triangles in 3D
 * @see Mesh
 * @note Works for 'small' triangles only - i.e. there MUST NOT be any
 *       significant curvature across each triangle of the mesh.
 */
class Mesh3D : public Mesh<Dimension{3}> {
  public:
    /** @brief default constructor */
    Mesh3D() = default;

    /** @brief copy constructor */
    Mesh3D(const Mesh3D&) = delete;

    /** @brief move constructor */
    Mesh3D(Mesh3D&&) = default;

    /** @brief copy assignment */
    Mesh3D& operator=(const Mesh3D&) = delete;

    /** @brief move assignment */
    Mesh3D& operator=(Mesh3D&&) = default;

    ~Mesh3D() override = default;

    /**
     * @see Entity::hit_record
     * @note Works for 'small' triangles only - i.e. there MUST NOT be any
     *       significant curvature across each triangle of the mesh.
     */
    HitRecord3D hit_record(const Geometry3D& geometry,
                           const RaySegment3D& ray_segment,
                           const Scalar t_min = 0.0) const override;

  private:
    /**
     * @brief returns u-v coordinates of x in plane spanned by b1 and b2
     * @note It is asserted that x is in the plane spanned by b1 and b2.
     * @note `x = u * b1 + v * b2`
     * @note This is solving for linear coefficients.
     */
    static constexpr std::pair<Scalar, Scalar>
    uv_tri_coords(const Vec3& b1, const Vec3& b2, const Vec3& x) {
        using namespace tensor;

        const Scalar b1b1 = dot(b1, b1);
        const Scalar b1b2 = dot(b1, b2);
        const Scalar b2b2 = dot(b2, b2);
        const Scalar D = b1b1 * b2b2 - b1b2 * b1b2;
        const Scalar b1x = dot(b1, x);
        const Scalar b2x = dot(b2, x);
        const Scalar u = (b1b1 * b2x - b1b2 * b1x) / D;
        const Scalar v = (b2b2 * b1x - b1b2 * b2x) / D;
        return {u, v};
    }

    HitRecord3D face_hit_record(const Geometry3D& geometry,
                                const RaySegment3D& ray_segment,
                                const Scalar t_min, const Vec3& point0,
                                const Vec3& point1, const Vec3& point2) const {

        using namespace tensor;
        // basis for span

        const Vec3 b1 = point1 - point0;
        const Vec3 b2 = point2 - point0;
        // pseudo-normal (NOT the face normal)
        const Vec3 n = unit_vector(cross(b1, b2));
        // level parameter
        const Scalar l = dot(n, point0);

        // ray
        const Vec3 s = ray_segment.start();
        const Vec3 d = ray_segment.direction();

        // intersection parameters
        const Scalar a = l - dot(s, n);
        const Scalar b = dot(d, n);

        if (b == 0.0) {
            // ray 'parallel' to face
            return {.t = infinity};
        }

        const Scalar t = a / b;

        if (t < t_min || t >= ray_segment.t_max()) {
            // outside of ray segment
            return {.t = infinity};
        }

        const auto [u, v] = uv_tri_coords(b1, b2, (s + t * d) - point0);

        if (u < 0.0 || v < 0.0 || u + v > 1.0) {
            // outside of triangle region
            return {.t = infinity};
        }

        // construct hit record
        const Vec3 point = ray_segment.at(t);
        const Mat3x3 metric = geometry.metric(point);
        const Mat3x3 to_onb_jacobian = geometry.to_onb_jacobian(point);

        HitRecord3D record;
        record.t = t;
        record.point = point;
        record.uv_coordinates = {u, v};
        // note: The normal is position dependent since the tri might be curved.
        // note: Calculating a face normal from the cross product of two
        //       coordinate deltas (as given here) works only, if the curvature
        //       deltas are approximately the same as the local tangent vectors
        //       in the sme direction. This is only true, if the CURVATURE AROSS
        //       THE SURFACE is NOT SIGNIFICANT!
        //       In general, the local tangential vectors t1 and t2 are the
        //       local tangents of the two geodesics running through the hit
        //       point and `point1` and `pont2` respectively.
        Vec3 normal = cross(metric * b1, metric * b2);
        // normalize
        normal *= Scalar{1} / std::sqrt((dot(normal, metric * normal)));
        record.set_face_normal(to_onb_jacobian, metric, d, normal);
        record.material = this->material.get();
        return record;
    }
};

HitRecord3D Mesh3D::hit_record(const Geometry3D& geometry,
                               const RaySegment3D& ray_segment,
                               const Scalar t_min) const {
    HitRecord3D record = {.t = infinity};
    for (const auto& face : this->faces) {
        HitRecord3D rec =
            face_hit_record(geometry, ray_segment, t_min, this->points[face[0]],
                            this->points[face[1]], this->points[face[2]]);
        if (rec.t < record.t) {
            record = rec;
        }
    }
    return record;
}

/**
 * @brief face of a 3D mesh object (triplet of point indices)
 * @see Mesh::Face
 */
using Face3D = Mesh3D::Face;

} // namespace cpp_raytracing

#endif
