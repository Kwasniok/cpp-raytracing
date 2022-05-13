/**
 * @file
 * @brief diffuse IO
 */

#ifndef CPP_RAYTRACING_IO_MATERIALS_DIFFUSE_HPP
#define CPP_RAYTRACING_IO_MATERIALS_DIFFUSE_HPP

#include "../../materials/diffuse.hpp"
#include "../color.hpp"
#include "../identifier.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct mat_diffuse_prefix : pegtl::string<'D', 'i', 'f', 'f', 'u', 's', 'e'> {};
struct mat_diffuse_id : property<identifier, 'i', 'd'> {};
struct mat_diffuse_color : property<color, 'c', 'o', 'l', 'o', 'r'> {};
struct mat_diffuse_properties : tuple<mat_diffuse_id, mat_diffuse_color> {};
/** @brief grammar for cpp_raytracing::Diffuse */
struct mat_diffuse
    : pegtl::seq<mat_diffuse_prefix, pegtl::pad<mat_diffuse_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Diffuse */
template <>
struct grammar_for<Diffuse> {
    /** @brief grammar to be selected */
    using get = mat_diffuse;
};

/** @brief selector of cpp_raytracing::Diffuse for partial parse tree */
template <>
struct selector<mat_diffuse> : remove_content {};
template <>
struct selector<mat_diffuse_color> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Diffuse to a stream */
template <>
void write<Diffuse>(std::ostream& os, const Diffuse& val) {
    os << "Diffuse ";
    write_tuple(os, Property{"id", val.id}, Property{"color", val.color});
}

/**
 *@brief parse a cpp_raytracing::Diffuse from a node tree
 */
template <>
Diffuse parse_node(const tao::pegtl::parse_tree::node& node) {
    Identifier<Material> id;
    Color color = Colors::BLACK;
    for (const auto& child : node.children) {
        if (child->is_type<grammar::mat_diffuse_id>()) {
            id = parse_node<Identifier<Material>>(*(child->children[0]));
        }
        if (child->is_type<grammar::mat_diffuse_color>()) {
            color = parse_node<Color>(*(child->children[0]));
        }
    }
    // assert success
    return Diffuse{std::move(id), color};
}

}} // namespace cpp_raytracing::io

#endif
