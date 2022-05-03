/**
 * @file
 * @brief Color IO
 */

#ifndef CPP_RAYTRACING_IO_COLOR_HPP
#define CPP_RAYTRACING_IO_COLOR_HPP

#include "../color.hpp"
#include "base.hpp"
#include "scalar.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

/** @brief grammar for cpp_raytracing::Color */
struct color : tuple<scalar, scalar, scalar> {};

/** @brief grammar for parsing a cpp_raytracing::Color */
template <>
struct grammar_for<Color> {
    /** @brief grammar to be selected */
    using get = color;
};

/** @brief selector of cpp_raytracing::Color for partial parse tree */
template <>
struct selector<color> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Color to a stream */
template <>
void write<Color>(std::ostream& os, const Color& val) {
    write_tuple(os, val.r(), val.g(), val.b());
}

/**
 *@brief parse a cpp_raytracing::Color from a node tree
 */
template <>
Color parse_node(const tao::pegtl::parse_tree::node& node) {
    const tao::pegtl::parse_tree::node& node_r = *(node.children[0]);
    const auto r = parse_node<Scalar>(node_r);
    const tao::pegtl::parse_tree::node& node_g = *(node.children[1]);
    const auto g = parse_node<Scalar>(node_g);
    const tao::pegtl::parse_tree::node& node_b = *(node.children[2]);
    const auto b = parse_node<Scalar>(node_b);
    return {r, g, b};
}

}} // namespace cpp_raytracing::io

#endif
