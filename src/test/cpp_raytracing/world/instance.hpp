/**
 * @file
 * @brief instance
 */

#ifndef CPP_RAYTRACING_TEST_ENTITIES_INSTANCE_HPP
#define CPP_RAYTRACING_TEST_ENTITIES_INSTANCE_HPP

#include <cmath>
#include <memory>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/entities/entity.hpp>

namespace cpp_raytracing { namespace test {

/**
 * @brief represents an instance
 * @note From instance space to parent space the affine transfomation is
 *       performed in order: scaling, rotation, translation. All parameters
 *       (position, rotation, scale) describe these transformations in this
 *       direction.
 * @note Asserts Euclidean Geometry and ignores geometry parameter.
 */
class Instance : public Entity {
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
    std::shared_ptr<Entity> entity;

    /** @brief default construct with default idenfifier root */
    Instance() = default;
    /** @brief move constructor */
    Instance(Instance&& other) = default;
    /** @brief move assignment */
    Instance& operator=(Instance&& other) = default;

    virtual ~Instance() = default;

    virtual void set_time(const Scalar time) override;

    virtual HitRecord hit_record(const Geometry& geometry,
                                 const RaySegment& ray,
                                 const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

    /**
     * @brief clone an instanc
     * @note The clone is identical up to its id;
     * @see Identifier::clone
     */
    Instance clone() const {
        Instance other;
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
    Mat3x3 _transformation = Mat3x3::identity();
    /**
     * @brief cached inverse rotation and scaling
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat3x3 _inv_transformation = Mat3x3::identity();
};

void Instance::set_time(const Scalar time) {
    // super call
    this->Entity::set_time(time);

    // cache transformation if nedded
    if (entity) {
        entity->set_time(time);
        _transformation = rotation_mat(rotation) * scaling_mat(scale);
        _inv_transformation =
            inverse_scaling_mat(scale) * inverse_rotation_mat(rotation);
    }
}

HitRecord Instance::hit_record(const Geometry& geometry, const RaySegment& ray,
                               const Scalar t_min, const Scalar t_max) const {
    if (entity) {
        // inverse transform ray to instance space, calculate hit record and
        // transform hit record

        // affine
        const Vec3 start = _inv_transformation * (ray.start() - position);
        // linear
        const Vec3 direction = _inv_transformation * ray.direction();

        HitRecord record = entity->hit_record(
            geometry, RaySegment{start, direction}, t_min, t_max);

        if (record.t < infinity) {

            record.point = _transformation * record.point + position;
            record.normal = unit_vector(_transformation * record.normal);
        }
        return record;
    } else {
        return {.t = infinity};
    }
}

std::optional<AxisAlignedBoundingBox> Instance::bounding_box() const {
    if (entity) {
        const auto bounds = entity->bounding_box();
        if (bounds) {
            const Scalar x[2] = {bounds->min().x(), bounds->max().x()};
            const Scalar y[2] = {bounds->min().y(), bounds->max().y()};
            const Scalar z[2] = {bounds->min().z(), bounds->max().z()};

            // linear transformation (translation is deferred)
            const Mat3x3 transformation =
                rotation_mat(rotation) * scaling_mat(scale);

            // transform all corners and select min/max coefficients
            Vec3 min{infinity, infinity, infinity};
            Vec3 max{-infinity, -infinity, -infinity};
            for (auto i = 0; i < 2; ++i) {
                for (auto j = 0; j < 2; ++j) {
                    for (auto k = 0; k < 2; ++k) {
                        const Vec3 corner =
                            transformation * Vec3{x[i], y[j], z[k]};
                        min = Vec3{
                            std::min(min.x(), corner.x()),
                            std::min(min.y(), corner.y()),
                            std::min(min.z(), corner.z()),
                        };
                        max = Vec3{
                            std::max(max.x(), corner.x()),
                            std::max(max.y(), corner.y()),
                            std::max(max.z(), corner.z()),
                        };
                    }
                }
            }

            // complete affine transformation via translation
            return AxisAlignedBoundingBox{min + position, max + position};
        }
    }
    return std::nullopt;
}

}} // namespace cpp_raytracing::test

#endif
