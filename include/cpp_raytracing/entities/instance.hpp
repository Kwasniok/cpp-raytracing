/**
 * @file
 * @brief instance
 */

#ifndef CPP_RAYTRACING_ENTITIES_INSTANCE_HPP
#define CPP_RAYTRACING_ENTITIES_INSTANCE_HPP

#include <memory>

#include "../math.hpp"
#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief represents an instance
 */
class Instance : public Entity {
  public:
    /** @brief translation */
    Vec3 position = {0.0, 0.0, 0.0};

    /** @brief instanced entity */
    std::shared_ptr<Entity> entity;

    /** @brief default construct with default idenfifier root */
    Instance() = default;
    /** @brief move constructor */
    Instance(Instance&& other) = default;
    /** @brief move assignment */
    Instance& operator=(Instance&& other) = default;

    virtual ~Instance() = default;

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
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
};

/** @brief default identifier for instances */
template <>
struct default_identifier<Instance> {
    /** @brief default identifier for instances */
    static constexpr const char* value = "instance";
};

HitRecord Instance::hit_record(const Ray& ray, const Scalar t_min,
                               const Scalar t_max) const {
    if (entity) {
        const Ray r{ray.start() - position, ray.direction()};
        HitRecord record = entity->hit_record(r, t_min, t_max);
        record.point += position;
        return record;
    } else {
        return {.t = infinity};
    }
}

std::optional<AxisAlignedBoundingBox> Instance::bounding_box() const {
    if (entity) {
        const auto bounds = entity->bounding_box();
        if (bounds) {
            return *bounds + position;
        }
    }
    return std::nullopt;
}

} // namespace cpp_raytracing

#endif
