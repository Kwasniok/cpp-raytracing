/**
 * @file
 * @brief instance for embedded Cartesian geometries
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_CARTESIAN_EMBEDDED_INSTANCE_HPP
#define CPP_RAYTRACING_GEOMETRIES_CARTESIAN_EMBEDDED_INSTANCE_HPP

#include <algorithm>
#include <cmath>
#include <memory>

#include <cpp_raytracing/values/tensor.hpp>
#include <cpp_raytracing/world/entities/base.hpp>

namespace cpp_raytracing { namespace cartesian_embedded {

/**
 * @brief represents an n-dimensinal instance for embedded Cartesian geometries
 * @note From instance space to parent space the affine transfomation is
 *       performed in order: scaling, translation. All parameters
 *       (position, scale) describe these transformations in this
 *       direction.
 * @note Asserts Cartesian Geometry.
 */
template <Dimension DIMENSION>
class Instance : public Entity<DIMENSION> {
  public:
    /** @brief translation */
    Vec<DIMENSION> position = {};
    /** @brief scaling */
    Vec<DIMENSION> scale{};

    /** @brief instanced entity */
    std::shared_ptr<Entity<DIMENSION>> entity;

    /** @brief default construct with default idenfifier root */
    Instance() { std::ranges::fill(scale, 1.0); }
    /** @brief copy constructor */
    Instance(const Instance& other) = delete;
    /** @brief move constructor */
    Instance(Instance&& other) = default;
    /** @brief copy assignment */
    Instance& operator=(const Instance& other) = delete;
    /** @brief move assignment */
    Instance& operator=(Instance&& other) = default;

    ~Instance() override = default;

    void set_time(const Scalar time) override;

    /** @see Entity::hit_record */
    HitRecord<DIMENSION> hit_record(const Geometry<DIMENSION>& geometry,
                                    const RaySegment<DIMENSION>& ray_segment,
                                    const Scalar t_min = 0.0) const override;

    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override;

    /**
     * @brief clone an instance
     * @note The clone is identical up to its id;
     * @see Identifier::clone
     */
    Instance<DIMENSION> clone() const {
        Instance<DIMENSION> other;
        other.id = this->id.clone();
        other.position = this->position;
        other.scale = this->scale;
        other.entity = this->entity;
        return other;
    }

  private:
    /**
     * @brief cached scaling
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat<DIMENSION> _transformation{};

    /**
     * @brief cached scaling for 3D subspace
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat<3_D> _onb_transformation{};
    /**
     * @brief cached inverse scaling
     * @note Updated by set_time if an entity is present.
     * @see set_time
     */
    Mat<DIMENSION> _inv_transformation{};
};

template <Dimension DIMENSION>
void Instance<DIMENSION>::set_time(const Scalar time) {
    using namespace tensor;

    // super call
    this->Entity<DIMENSION>::set_time(time);

    // cache transformation if nedded
    if (entity) {
        entity->set_time(time);
        const Vec<3_D> onb_scale = {scale[0], scale[1], scale[2]};
        _transformation = scaling_mat<DIMENSION>(scale);
        _onb_transformation = scaling_mat<3_D>(onb_scale);
        _inv_transformation = inverse_scaling_mat<DIMENSION>(scale);
    }
}

template <Dimension DIMENSION>
HitRecord<DIMENSION>
Instance<DIMENSION>::hit_record(const Geometry<DIMENSION>& geometry,
                                const RaySegment<DIMENSION>& ray_segment,
                                const Scalar t_min) const {

    using namespace tensor;

    if (entity) {
        // inverse transform ray to instance space, calculate hit record and
        // transform hit record

        // affine
        const Vec<DIMENSION> start =
            _inv_transformation * (ray_segment.start() - position);
        // linear
        const Vec<DIMENSION> direction =
            _inv_transformation * ray_segment.direction();

        HitRecord<DIMENSION> record = entity->hit_record(
            geometry,
            RaySegment<DIMENSION>{start, direction, ray_segment.t_max()},
            t_min);

        if (record.t < infinity) {

            record.point = _transformation * record.point + position;
            record.onb_normal =
                unit_vector(_onb_transformation * record.onb_normal);
        }
        return record;
    } else {
        return {.t = infinity};
    }
}

template <Dimension DIMENSION>
std::optional<AxisAlignedBoundingBox<DIMENSION>>
Instance<DIMENSION>::bounding_box() const {
    using namespace tensor;

    if (entity) {
        const auto bounds = entity->bounding_box();
        if (bounds) {
            return AxisAlignedBoundingBox<DIMENSION>{
                _transformation * bounds->min() + position,
                _transformation * bounds->max() + position,
            };
        }
    }
    return std::nullopt;
}

}} // namespace cpp_raytracing::cartesian_embedded

#endif
