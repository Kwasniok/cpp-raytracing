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
};

class Hittable {
  public:
    virtual HitRecord hit_record(const Ray& ray, const Scalar t_min = 0.0,
                                 const Scalar t_max = SCALAR_INF) const = 0;
};

} // namespace ray

#endif
