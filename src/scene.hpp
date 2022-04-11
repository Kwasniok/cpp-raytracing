#ifndef CPP_RAYTRACING_SCENE_H
#define CPP_RAYTRACING_SCENE_H

#include <memory>
#include <vector>

#include "hittable.hpp"

namespace ray {

using Scene = std::vector<std::unique_ptr<Hittable>>;

}

#endif
