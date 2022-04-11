#ifndef CPP_RAYTRACING_SHPERE_H
#define CPP_RAYTRACING_SHPERE_H

#include "hittable.hpp"
#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

class Sphere : public Hittable {
  public:
    constexpr Sphere() : _origin(Vec3(0.0, 0.0, 0.0)), _radius(1.0){};
    constexpr Sphere(const Vec3& origin, const Scalar radius)
        : _origin(origin), _radius(radius) {}

    constexpr Vec3 origin() const { return _origin; }
    constexpr Scalar radius() const { return _radius; }

    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min,
                                 const Scalar t_max) const override;

  public:
    Vec3 _origin;
    Scalar _radius;
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
    return HitRecord{
        .point = point,
        .normal = normal,
        .t = t,
    };
    ;
}

} // namespace ray

#endif
