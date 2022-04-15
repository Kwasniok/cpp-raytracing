#ifndef CPP_RAYTRACING_SHADER_H
#define CPP_RAYTRACING_SHADER_H

#include "color.hpp"
#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

struct Material {
    Color diffuse_color = Colors::WHITE;
    Color reflection_color = Colors::WHITE;
    Scalar absorption = 1.0;   // 0.0 ... 1.0
    Scalar reflection = 0.0;   // 0.0 ... 1.0
    Scalar reflection_roughness = 0.0; // 0.0 ... 1.0
};

} // namespace ray

#endif
