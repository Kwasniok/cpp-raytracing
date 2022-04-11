#ifndef CPP_RAYTRACING_HITTABLE_H
#define CPP_RAYTRACING_HITTABLE_H

#include <limits>

#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

struct HitRecord {
    Vec3 point;
    Vec3 normal;
    Scalar t;
    bool front_face;

    void set_face_normal(const Ray& ray, const Vec3& face_normal) {
        front_face = dot(face_normal, ray.direction());
        normal = front_face ? face_normal : -face_normal;
    }
};

class Hittable {
  public:
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = SCALAR_INF) const = 0;
};

} // namespace ray

#endif
