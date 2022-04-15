#ifndef CPP_RAYTRACING_SHADER_H
#define CPP_RAYTRACING_SHADER_H

#include "color.hpp"
#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

struct Material {
    Color diffuse_color = Colors::WHITE;
};

} // namespace ray

#endif
