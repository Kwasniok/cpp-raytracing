/**
 * @file
 * @brief instance
 */

#ifndef CPP_RAYTRACING_TEST_ENTITIES_INSTANCE_HPP
#define CPP_RAYTRACING_TEST_ENTITIES_INSTANCE_HPP

#include <cmath>
#include <memory>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/entities/base.hpp>

namespace cpp_raytracing {

/**
 * @brief represents an instance
 * @note From instance space to parent space the affine transfomation is
 *       performed in order: scaling, rotation, translation. All parameters
 *       (position, rotation, scale) describe these transformations in this
 *       direction.
 * @note Asserts Euclidean Geometry and ignores geometry parameter.
 */
class Instance3D : public Entity3D {
  public:
    /** @brief translation */
    Vec3 position = {0.0, 0.0, 0.0};
    /**
     * @brief rotation (Trait-Bryan angles)
     * @note The application order is: roll, pitch, yaw
     * @see rotation_mat
     */
    Vec3 rotation = {0.0, 0.0, 0.0};
    /** @brief scaling */
    Vec3 scale = {1.0, 1.0, 1.0};

    /** @brief instanced entity */
    std::shared_ptr<Entity3D> entity;

    /** @brief default construct with default idenfifier root */
    Instance3D() = default;
    /** @brief copy constructor */
    Instance3D(const Instance3D& other) = delete;
    /** @brief move constructor */
    Instance3D(Instance3D&& other) = default;
    /** @brief copy assignment */
    Instance3D& operator=(const Instance3D& other) = delete;
    /** @brief move assignment */
    Instance3D& operator=(Instance3D&& other) = default;

    ~Instance3D() override = default;

    void set_time(const Scalar time) override;

    HitRecord hit_record(const Geometry& geometry,
                         const RaySegment3D& ray_segment,
                         const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox3D> bounding_box() const override;

    /**
     * @brief clone an instanc
     * @note The clone is identical up to its id;
     * @see Identifier::clone
     */
    Instance3D clone() const {
        Instance3D other;
        other.id = id.clone();
        other.position = position;
        other.entity = entity;
        return other;
    }

  private:
    /**
     * @brief cached scaling and roation
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat3x3 _transformation = tensor::identity_mat<3_D>;
    /**
     * @brief cached inverse rotation and scaling
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat3x3 _inv_transformation = tensor::identity_mat<3_D>;
};

void Instance3D::set_time(const Scalar time) {
    using namespace tensor;

    // super call
    this->Entity3D::set_time(time);

    // cache transformation if nedded
    if (entity) {
        entity->set_time(time);
        _transformation = rotation_mat(rotation) * scaling_mat<3_D>(scale);
        _inv_transformation =
            inverse_scaling_mat<3_D>(scale) * inverse_rotation_mat(rotation);
    }
}

HitRecord Instance3D::hit_record(const Geometry& geometry,
                                 const RaySegment3D& ray_segment,
                                 const Scalar t_min) const {

    using namespace tensor;

    if (entity) {
        // inverse transform ray to instance space, calculate hit record and
        // transform hit record

        // affine
        const Vec3 start =
            _inv_transformation * (ray_segment.start() - position);
        // linear
        const Vec3 direction = _inv_transformation * ray_segment.direction();

        HitRecord record = entity->hit_record(
            geometry, RaySegment3D{start, direction, ray_segment.t_max()},
            t_min);

        if (record.t < infinity) {

            record.point = _transformation * record.point + position;
            record.normal = unit_vector(_transformation * record.normal);
        }
        return record;
    } else {
        return {.t = infinity};
    }
}

std::optional<AxisAlignedBoundingBox3D> Instance3D::bounding_box() const {
    using namespace tensor;

    if (entity) {
        const auto bounds = entity->bounding_box();
        if (bounds) {
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
            const Scalar x[2] = {bounds->min()[0], bounds->max()[0]};
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
            const Scalar y[2] = {bounds->min()[1], bounds->max()[1]};
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
            const Scalar z[2] = {bounds->min()[2], bounds->max()[2]};

            // linear transformation (translation is deferred)
            const Mat3x3 transformation =
                rotation_mat(rotation) * scaling_mat<3_D>(scale);

            // transform all corners and select min/max coefficients
            Vec3 low{infinity, infinity, infinity};
            Vec3 high{-infinity, -infinity, -infinity};
            for (auto i = 0; i < 2; ++i) {
                for (auto j = 0; j < 2; ++j) {
                    for (auto k = 0; k < 2; ++k) {
                        const Vec3 corner =
                            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                            transformation * Vec3{x[i], y[j], z[k]};
                        low.inplace_elementwise(min, corner);
                        high.inplace_elementwise(max, corner);
                    }
                }
            }

            // complete affine transformation via translation
            return AxisAlignedBoundingBox3D{low + position, high + position};
        }
    }
    return std::nullopt;
}

} // namespace cpp_raytracing

#endif
