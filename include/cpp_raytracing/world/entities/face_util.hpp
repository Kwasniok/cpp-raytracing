/**
 * @file
 * @brief face util
 */

#ifndef CPP_RAYTRACING_ENTITIES_FACE_UTIL_HPP
#define CPP_RAYTRACING_ENTITIES_FACE_UTIL_HPP

#include <vector>

#include "../../values/tensor.hpp"

namespace cpp_raytracing {

/** @brief point index type */
using Index = typename std::vector<Vec<1_D>>::size_type;
/** @brief face geometry representation type (triplet of point indices)*/
using FaceIndicies = std::array<Index, 3>;
/** @brief uv map per face type */
struct UVMap {
    Mat2x2 m;
    Vec2 b;

    Vec2 operator()(const Vec2& uv) const {
        using namespace tensor;
        return m * uv + b;
    }
};
/** @brief face geometry + metadata type */
struct Face {
    FaceIndicies indices;
    UVMap uv_map = {tensor::identity_mat<2_D>, tensor::zero_vec<2_D>};
};

} // namespace cpp_raytracing

#endif