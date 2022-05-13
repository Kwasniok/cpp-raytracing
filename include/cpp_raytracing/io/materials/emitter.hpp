/**
 * @file
 * @brief emitter IO
 */

#ifndef CPP_RAYTRACING_IO_MATERIALS_EMITTER_HPP
#define CPP_RAYTRACING_IO_MATERIALS_EMITTER_HPP

#include "../../materials/emitter.hpp"
#include "../color.hpp"
#include "../identifier.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct mat_emitter_prefix : pegtl::string<'E', 'm', 'i', 't', 't', 'e', 'r'> {};
struct mat_emitter_id : property<identifier, 'i', 'd'> {};
struct mat_emitter_color : property<color, 'c', 'o', 'l', 'o', 'r'> {};
struct mat_emitter_properties : tuple<mat_emitter_id, mat_emitter_color> {};
/** @brief grammar for cpp_raytracing::Emitter */
struct mat_emitter
    : pegtl::seq<mat_emitter_prefix, pegtl::pad<mat_emitter_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Emitter */
template <>
struct grammar_for<Emitter> {
    /** @brief grammar to be selected */
    using get = mat_emitter;
};

/** @brief selector of cpp_raytracing::Emitter for partial parse tree */
template <>
struct selector<mat_emitter> : remove_content {};
template <>
struct selector<mat_emitter_color> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Emitter to a stream */
template <>
void write<Emitter>(std::ostream& os, const Emitter& val) {
    os << "Emitter ";
    write_tuple(os, Property{"id", val.id}, Property{"color", val.color});
}

/**
 *@brief parse a cpp_raytracing::Emitter from a node tree
 */
template <>
Emitter parse_node(const tao::pegtl::parse_tree::node& node) {
    Identifier<Material> id;
    Color color = Colors::BLACK;
    for (const auto& child : node.children) {
        if (child->is_type<grammar::mat_emitter_id>()) {
            id = parse_node<Identifier<Material>>(*(child->children[0]));
        }
        if (child->is_type<grammar::mat_emitter_color>()) {
            color = parse_node<Color>(*(child->children[0]));
        }
    }
    // assert success
    return Emitter{std::move(id), color};
}

}} // namespace cpp_raytracing::io

#endif
