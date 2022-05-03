/**
 * @file
 * @brief Vec3 IO
 */

#ifndef CPP_RAYTRACING_IO_VEC3_HPP
#define CPP_RAYTRACING_IO_VEC3_HPP

#include "../vec3.hpp"
#include "base.hpp"
#include "scalar.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

/** @brief grammar for cpp_raytracing::Vec3 */
struct vec3 : tuple<scalar, scalar, scalar> {};

/** @brief grammar for parsing a cpp_raytracing::Vec3 */
template <>
struct grammar_for<Vec3> {
    /** @brief grammar to be selected */
    using get = vec3;
};

/** @brief selector of cpp_raytracing::Vec3 for partial parse tree */
template <>
struct selector<vec3> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Vec3 to a stream */
template <>
void write<Vec3>(std::ostream& os, const Vec3& val) {
    write_tuple(os, val.x(), val.y(), val.z());
}

/**
 *@brief parse a cpp_raytracing::Vec3 from a node tree
 */
template <>
Vec3 parse_node(const tao::pegtl::parse_tree::node& node) {
    const tao::pegtl::parse_tree::node& node_x = *(node.children[0]);
    const auto x = parse_node<Scalar>(node_x);
    const tao::pegtl::parse_tree::node& node_y = *(node.children[1]);
    const auto y = parse_node<Scalar>(node_y);
    const tao::pegtl::parse_tree::node& node_z = *(node.children[2]);
    const auto z = parse_node<Scalar>(node_z);
    return {x, y, z};
}

}} // namespace cpp_raytracing::io

#endif
