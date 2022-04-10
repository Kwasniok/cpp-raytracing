#ifndef CPP_RAYTRACING_SHPERE_H
#define CPP_RAYTRACING_SHPERE_H

#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

class Sphere {
  public:
    constexpr Sphere(const Vec3& origin, const Scalar radius)
        : _origin(origin), _radius(radius) {}

    constexpr Vec3 origin() const { return _origin; }
    constexpr Scalar radius() const { return _radius; }

  public:
    Vec3 _origin;
    Scalar _radius;
};

} // namespace ray

#endif
