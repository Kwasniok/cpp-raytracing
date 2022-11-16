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

    /** @brief intersection point of ray and object */
    VolumeVec point{};

    /**
     * @brief surface normal pointing towards 'exterior' relative to local 3D
     *        ONB tangental space of point
     */
    Vec3 onb_normal{};

    /**
     * @brief direction of incoming ray relative to local 3D ONB tangental space
     *        of point
     */
    Vec3 onb_ray_direction{};

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
     * @param onb_ray_direction ray direction in local ONB tangential space
     * @param onb_normal face normal in local ONB tangential space
     * @note implicitly sets #front_face
     * @note The local 3D ONB tangential space is defined by the Geometry.
     * @see Geometry::to_onb_jacobian, Geometry::from_onb_jacobian
     */
    void set_local_geometry(const Vec3& onb_ray_direction,
                            const Vec3& onb_normal) {
        using namespace tensor;
        this->onb_ray_direction = onb_ray_direction;
        this->front_face = dot(onb_normal, onb_ray_direction) < 0.0;
        this->onb_normal = front_face ? onb_normal : -onb_normal;
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
