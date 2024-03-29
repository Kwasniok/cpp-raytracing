/**
 * @file
 * @brief entity
 */

#ifndef CPP_RAYTRACING_ENTITIES_ENTITY_HPP
#define CPP_RAYTRACING_ENTITIES_ENTITY_HPP

#include <memory>
#include <optional>

#include "../../values/identifier.hpp"
#include "../../values/scalar.hpp"
#include "../../values/tensor.hpp"
#include "../../world/geometries/base.hpp"
#include "../animators/base.hpp"
#include "../axis_aligned_bounding_box.hpp"
#include "../hit_record.hpp"

namespace cpp_raytracing {

/**
 * @brief represents an entity
 */
template <Dimension DIMENSION>
class Entity {
  public:
    /** @brief unique entity identifier */
    Identifier<class Entity> id;

    /** @brief default construct with default idenfifier root */
    Entity() = default;

    /** @brief copy constructor */
    Entity(const Entity&) = delete;

    /** @brief move constructor */
    Entity(Entity&& other) = default;

    /** @brief copy assignment */
    Entity& operator=(const Entity&) = delete;

    /** @brief move assignment */
    Entity& operator=(Entity&& other) = default;

    virtual ~Entity() = default;

    /**
     * @brief sets the animator
     * @note none removes the animator and means the entity is static
     */
    virtual void set_animator(std::unique_ptr<Animator<DIMENSION>>&& animator) {
        _animator = std::move(animator);
    }
    /** @brief returns the animator of the entity */
    virtual Animator<DIMENSION>& animator() { return *_animator; }

    /**
     * @brief requests entity to take its appearence for the given time
     * @note A static entity might ignore this entirely.
     */
    virtual void set_time(const Scalar time) {
        if (_animator) {
            _animator->update_for_time(time, this);
        }
    }

    /**
     * @brief calculates the intersection of the light ray with the object
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * ::infinity
     */
    virtual HitRecord<DIMENSION>
    hit_record(const Geometry<DIMENSION>& geometry,
               const RaySegment<DIMENSION>& ray_segment,
               const Scalar t_min = 0.0) const = 0;

    /**
     * @brief might return a boundaring box if the entity is bounded
     * @note If none, the entity might be infinitely large or have no appreance
     *       at all.
     */
    virtual std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const {
        return std::nullopt;
    }

    /**
     * @brief returns true if entity is bounded
     * @note It MUST be equal to `bounding_box().has_value()` and might be
     *       redefined for optimization only!
     */
    virtual bool is_bounded() const { return bounding_box().has_value(); }

  private:
    /**
     * @brief animator of the entity (optional)
     * @note none means the entity is static.
     */
    std::unique_ptr<Animator<DIMENSION>> _animator;
};

/** @brief default identifier for entitys */
template <Dimension DIMENSION>
struct default_identifier<Entity<DIMENSION>> {
    /** @brief default identifier for entitys */
    static constexpr const char* value = "entity";
};

/** @brief 3D entity interface */
using Entity3D = Entity<Dimension{3}>;

} // namespace cpp_raytracing

#endif
