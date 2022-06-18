/**
 * @file
 * @brief hittable planar object
 */

#ifndef CPP_RAYTRACING_ENTITIES_PLANE_HPP
#define CPP_RAYTRACING_ENTITIES_PLANE_HPP

#include "entity.hpp"

namespace cpp_raytracing {

/**
 * @brief planar object
 * @note centered unit square in of x-y plane facing upwards
 */
class Plane : public Entity {
  public:
    /** @brief true iff plane is limited by `x >= -0.5` */
    bool finite_neg_x = true;
    /** @brief true iff plane is limited by `x <= +0.5` */
    bool finite_pos_x = true;
    /** @brief true iff plane is limited by `y >= -0.5` */
    bool finite_neg_y = true;
    /** @brief true iff plane is limited by `y <= +0.5` */
    bool finite_pos_y = true;
    /** @brief material of the plane */
    std::shared_ptr<Material> material;

    virtual ~Plane() = default;

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = infinity) const override;

    virtual std::optional<AxisAlignedBoundingBox> bounding_box() const override;

  private:
    static constexpr Scalar finite_x_min = -1.0;
    static constexpr Scalar finite_x_max = +1.0;
    static constexpr Scalar finite_y_min = -1.0;
    static constexpr Scalar finite_y_max = +1.0;
};

HitRecord Plane::hit_record(const Ray& ray, const Scalar t_min,
                            const Scalar t_max) const {

    if (ray.direction().z() == 0.0) {
        return {.t = infinity};
    }
    const Scalar t = -ray.start().z() / ray.direction().z();
    if (t < t_min || t > t_max) {
        return {.t = infinity};
    }

    const Vec3 point = ray.at(t);
    if ((finite_neg_x && point.x() < finite_x_min) ||
        (finite_pos_x && point.x() > finite_x_max) ||
        (finite_neg_y && point.y() < finite_y_min) ||
        (finite_pos_y && point.y() > finite_y_max)) {
        return {.t = infinity};
    }

    HitRecord record;
    record.t = t;
    record.point = point;
    record.uv_coordinates = Vec2{point.x(), point.y()};
    record.set_face_normal(ray, Vec3{0.0, 0.0, 1.0});
    record.material = material.get();
    return record;
}

std::optional<AxisAlignedBoundingBox> Plane::bounding_box() const {
    return AxisAlignedBoundingBox{
        Vec3{
            finite_neg_x ? finite_x_min : -infinity,
            finite_neg_y ? finite_y_min : -infinity,
            -1e-8,
        },
        Vec3{
            finite_pos_x ? finite_x_max : infinity,
            finite_pos_y ? finite_y_max : infinity,
            1e-8,
        },
    };
}

} // namespace cpp_raytracing

#endif
