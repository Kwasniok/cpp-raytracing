/**
 * @file
 * @brief module for hit records
 */

// NOTE: This has its own file to reduce dependencies.

#ifndef CPP_RAYTRACING_HIT_RECORD_HPP
#define CPP_RAYTRACING_HIT_RECORD_HPP

#include <memory>

#include "../values/scalar.hpp"
#include "../values/tensor.hpp"
#include "ray_segment.hpp"

namespace cpp_raytracing {

template <Dimension DIMENSION>
class Material;

/**
 * @brief records all information of a ray hitting (part of) an object
 * @note mediates between hittables and materials
 */
template <Dimension DIMENSION>
requires(DIMENSION >= 3) struct HitRecord {

    /** @brief space vector type */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief metric type  */
    using Metric = Mat<Dimension{3}, Dimension{3}>;

    /** @brief Jacobian type for to ONB conversions */
    using ToONBJac = Mat<Dimension{3}, DIMENSION>;
    /** @brief Jacobian type for from ONB conversions */
    using FromONBJac = Mat<DIMENSION, Dimension{3}>;

    /** @brief intersection point of ray and object */
    VolumeVec point{};

    /** @brief metric of the maifold at point */
    Metric metric{};
    /**
     * @brief Jacobian transforming a tangential vector of the manifold to a 3D
     *        vector relative to a local ONB tangential space definde by the
     *        geometry.
     * @note The tangential space is that of point.
     */

    ToONBJac to_onb_jacobian{};
    /**
     * @brief Jacobian transforming from a 3D vector relative to a local ONB
     *        tangential space definde by the geometry to a tangential vector of
     *        the manifold.
     * @note The tangential space is that of point.
     */
    ToONBJac from_onb_jacobian{};
    /**
     * @brief surface normal pointing towards 'exterior' relative to
     *        othronormal basis
     * @note For independence of the specific geometry all directions must be
     *       converted to a flat space via the local Jacobian at #point.
     */
    Vec3 onb_normal{};
    /** @brief texture coordinates */
    Vec2 uv_coordinates{};
    /** @brief material of the object */
    const Material<DIMENSION>* material = nullptr;
    /** @brief RaySegment3D parameter of #point */
    Scalar t = 0.0;
    /** @brief ray his surface from the outside */
    bool front_face = false;

    /**
     * @brief sets all quantites related to the local tangential space
     * @param metric local metric
     * @param to_onb_jacobian transforms to local ortho-normal base
     * @param from_onb_jacobian transforms to local ortho-normal base
     * @param ray_direction ray direction in local tangential space
     * @param face_normal face normal in local tangential space
     * @note face_normal will be converted to the local ortho-normal space
     *        when stored as #normal.
     */
    void set_local_geometry(const Metric& metric,
                            const ToONBJac& to_onb_jacobian,
                            const FromONBJac& from_onb_jacobian,
                            const VolumeVec& ray_direction,
                            const VolumeVec& face_normal) {
        using namespace tensor;

        this->metric = metric;
        this->to_onb_jacobian = to_onb_jacobian;
        this->from_onb_jacobian = from_onb_jacobian;

        front_face = dot(face_normal, metric * ray_direction) < 0.0;
        onb_normal = to_onb_jacobian * face_normal;
        onb_normal = front_face ? onb_normal : -onb_normal;
    }

    /**
     * @brief returns true iff t is finite
     * @note Used for debugging and testing.
     */
    bool hits() const { return t < infinity; }
};

/** @brief 3D hit record */
using HitRecord3D = HitRecord<Dimension{3}>;

} // namespace cpp_raytracing

#endif
