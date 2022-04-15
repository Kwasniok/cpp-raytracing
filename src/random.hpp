#ifndef CPP_RAYTRACING_RANDOM_H
#define CPP_RAYTRACING_RANDOM_H

#include <cmath>
#include <random>
#include <utility>

#include "scalar.hpp"

namespace ray {

inline Scalar random_scalar(Scalar min, Scalar max) {
    static std::uniform_real_distribution<Scalar> distribution(-1.0, 1.0);
    static std::mt19937 generator;
    return min + (max - min) * distribution(generator);
}

} // namespace ray

#endif
