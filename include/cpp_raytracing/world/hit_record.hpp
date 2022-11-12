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

class Material;

/**
 * @brief records all information of a ray hitting (part of) an object
 * @note mediates between hittables and materials
 */
template <Dimension DIMENSION>
requires(DIMENSION >= 3) struct HitRecord {

    /** @brief space ector type */
    using VolumeVec = Vec<DIMENSION>;

    /** @brief Jacobian type for to ONB conversions */
    using ToONBJac = Mat<Dimension{3}, DIMENSION>;
    /** @brief Jacobian type for from ONB conversions */
    using FromONBJac = Mat<DIMENSION, Dimension{3}>;
    /** @brief metric type  */
    using Metric = Mat<Dimension{3}, Dimension{3}>;

    /** @brief intersection point of ray and object */
    VolumeVec point{};
    /**
     * @brief surface normal pointing towards 'exterior' relative to
     *        othronormal basis
     * @note For independence of the specific geometry all directions must be
     *       converted to a flat space via the local Jacobian at #point.
     */
    Vec3 normal{};
    /** @brief texture coordinates */
    Vec2 uv_coordinates{};
    /** @brief material of the object */
    const Material* material = nullptr;
    /** @brief RaySegment3D parameter of #point */
    Scalar t = 0.0;
    /** @brief ray his surface from the outside */
    bool front_face = false;

    /**
     * @brief sets normal and front_face
     * @param to_onb_jacobian transforms to local ortho-normal base
     * @param metric local metric
     * @param ray_direction ray direction in local tangential space
     * @param face_normal face normal in local tangential space
     * @note face_normal will be converted to the local ortho-normal space
     *        when stored as #normal.
     */
    void set_face_normal(const ToONBJac& to_onb_jacobian, const Metric& metric,
                         const VolumeVec& ray_direction,
                         const VolumeVec& face_normal) {
        using namespace tensor;

        front_face = dot(face_normal, metric * ray_direction) < 0.0;
        normal = front_face ? face_normal : -face_normal;
        normal = to_onb_jacobian * normal;
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
