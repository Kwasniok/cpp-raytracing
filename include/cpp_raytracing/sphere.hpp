/**
 * @file
 * @brief spherical object representation
 */

#ifndef CPP_RAYTRACING_SHPERE_HPP
#define CPP_RAYTRACING_SHPERE_HPP

#include "hittable.hpp"
#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

/**
 * @brief spherical object
 */
class Sphere : public Hittable {
  public:
    /** @brief initialize as centered unit sphere */
    inline Sphere() : _origin(Vec3(0.0, 0.0, 0.0)), _radius(1.0){};
    /** @brief initialize with pprameters */
    inline Sphere(const Vec3& origin, const Scalar radius,
                  std::shared_ptr<Material> material)
        : _origin(origin), _radius(radius), _material(material) {}
    virtual ~Sphere() = default;

    /** @brief get origin of the sphere */
    inline Vec3 origin() const { return _origin; }
    /** @brief get radius of the sphere */
    inline Scalar radius() const { return _radius; }
    /** @brief get material of the sphere */
    inline std::shared_ptr<Material> material() const { return _material; }

    virtual HitRecord
    hit_record(const Ray& ray, const Scalar t_min = 0.0,
               const Scalar t_max = SCALAR_INF) const override;

  private:
    Vec3 _origin;
    Scalar _radius;
    std::shared_ptr<Material> _material;
};

HitRecord Sphere::hit_record(const Ray& ray, const Scalar t_min,
                             const Scalar t_max) const {
    // analytical geometry: line hits sphere
    // ray: s + t*d
    // sphere: (x-o)^2 = R^2
    // solve: a*t^2 + b*t + c = 0
    // where a = d^2 >= 0, b = 2*d*(s-o), c = (s-o)^2 - R^2
    // solution: t = (-b +/- sqrt(b^2 - 4ac))/(2a)
    const auto delta = ray.start() - origin();
    const auto a = dot(ray.direction(), ray.direction());
    const auto b_half = dot(ray.direction(), delta);
    const auto c = dot(delta, delta) - radius() * radius();
    const auto discriminant = b_half * b_half - a * c;
    if (discriminant < 0.0) {
        // no real solution
        return HitRecord{.t = SCALAR_INF};
    }

    // select minimal sloution in given range

    // t_+/- = (-b_half +/- sqrt(discriminant)) / a
    // t_->0, t_+>0: ray starts outside of the sphere and enters it at t_-
    // t_-<0, t_+>0: ray starts inside the sphere and leaves it at t_+
    // t_-<0, t_+<0: ray starts after the sphere and never enters it
    auto t = (-b_half - sqrt(discriminant)) / a;

    // select minimal positive sloution (if it exists)
    if (t < t_min || t > t_max) {
        t = (-b_half + sqrt(discriminant)) / a;
        if (t < t_min || t > t_max) {
            // no soltion in range
            return HitRecord{.t = SCALAR_INF};
        }
    }

    // found solution in range
    const Vec3 point = ray.at(t);
    const Vec3 normal = (point - origin()) / radius();
    HitRecord record;
    record.t = t;
    record.point = point;
    record.set_face_normal(ray, normal);
    record.material = material();
    return record;
}

} // namespace ray

#endif
