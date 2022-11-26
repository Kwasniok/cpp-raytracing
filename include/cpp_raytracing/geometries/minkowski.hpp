/**
 * @file
 * @brief Cartesian (3+1)-dimensional Minkowski geometries
 */

#ifndef CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_HPP
#define CPP_RAYTRACING_GEOMETRIES_MINKOWSKI_HPP

#include "cartesian_embedded/instance.hpp"
#include "cartesian_embedded/sphere.hpp"
#include "minkowski/camera.hpp"
#include "minkowski/geometry.hpp"

namespace cpp_raytracing {

/**@brief standard Minkowski space */
namespace minkowski {

/** @brief 4D instance */
using Instance4D = cartesian_embedded::Instance<Dimension{4}>;

/** @brief 4D sphere */
using Sphere4D = cartesian_embedded::Sphere<Dimension{4}>;

} // namespace minkowski

} // namespace cpp_raytracing

#endif
