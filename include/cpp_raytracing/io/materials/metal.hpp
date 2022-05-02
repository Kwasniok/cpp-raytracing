/**
 * @file
 * @brief metal IO
 */

#ifndef CPP_RAYTRACING_IO_MATERIALS_METAL_HPP
#define CPP_RAYTRACING_IO_MATERIALS_METAL_HPP

#include "../../materials/metal.hpp"
#include "../color.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct mat_metal_prefix : pegtl::string<'M', 'e', 't', 'a', 'l'> {};
struct mat_metal_color : property<color, 'c', 'o', 'l', 'o', 'r'> {};
struct mat_metal_roughness
    : property<scalar, 'r', 'o', 'u', 'g', 'h', 'n', 'e', 's', 's'> {};
struct mat_metal_properties : tuple<mat_metal_color, mat_metal_roughness> {
}; // TODO: relax order
/** @brief grammar for cpp_raytracing::Metal */
struct mat_metal
    : pegtl::seq<mat_metal_prefix, pegtl::pad<mat_metal_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Metal */
template <>
struct grammar_for<Metal> {
    /** @brief grammar to be selected */
    using get = mat_metal;
};

/** @brief selector of cpp_raytracing::Metal for partial parse tree */
template <>
struct selector<mat_metal> : std::true_type {};
template <>
struct selector<mat_metal_color> : std::true_type {};
template <>
struct selector<mat_metal_roughness> : std::true_type {};

} // namespace grammar

/** @brief write a cpp_raytracing::Metal to a stream */
template <>
void write<Metal>(std::ostream& os, const Metal& val) {
    os << "Metal {";
    os << "color = ";
    write(os, val.color);
    os << ", ";
    os << "roughness = ";
    write(os, val.roughness);
    os << "}";
}

/**
 *@brief parse a cpp_raytracing::Metal from a node tree
 */
template <>
Metal parse_node(const tao::pegtl::parse_tree::node& node) {
    Color color = Colors::BLACK;
    Scalar roughness = 0.0;
    for (const auto& child : node.children) {
        if (child->is_type<grammar::mat_metal_color>()) {
            color = parse_node<Color>(*(child->children[0]));
        }
        if (child->is_type<grammar::mat_metal_roughness>()) {
            roughness = parse_node<Scalar>(*(child->children[0]));
        }
    }
    // assert success
    return Metal{color, roughness};
}

}} // namespace cpp_raytracing::io

#endif
