/**
 * @file
 * @brief dielectric IO
 */

#ifndef CPP_RAYTRACING_IO_MATERIALS_DIELECTRIC_HPP
#define CPP_RAYTRACING_IO_MATERIALS_DIELECTRIC_HPP

#include "../../materials/dielectric.hpp"
#include "../color.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct mat_dielectric_prefix
    : pegtl::string<'D', 'i', 'e', 'l', 'e', 'c', 't', 'r', 'i', 'c'> {};
struct mat_dielectric_color : property<color, 'c', 'o', 'l', 'o', 'r'> {};
struct mat_dielectric_ior
    : property<scalar, 'i', 'n', 'd', 'e', 'x', '_', 'o', 'f', '_', 'r', 'e',
               'f', 'r', 'a', 'c', 't', 'i', 'o', 'n'> {};
struct mat_dielectric_properties
    : tuple<mat_dielectric_color, mat_dielectric_ior> {}; // TODO: relax order
/** @brief grammar for cpp_raytracing::Dielectric */
struct mat_dielectric : pegtl::seq<mat_dielectric_prefix,
                                   pegtl::pad<mat_dielectric_properties, ws>> {
};

/** @brief grammar for parsing a cpp_raytracing::Dielectric */
template <>
struct grammar_for<Dielectric> {
    /** @brief grammar to be selected */
    using get = mat_dielectric;
};

/** @brief selector of cpp_raytracing::Dielectric for partial parse tree */
template <>
struct selector<mat_dielectric> : remove_content {};
template <>
struct selector<mat_dielectric_color> : remove_content {};
template <>
struct selector<mat_dielectric_ior> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Dielectric to a stream */
template <>
void write<Dielectric>(std::ostream& os, const Dielectric& val) {
    os << "Dielectric ";
    write_tuple(os, Property{"color", val.color},
                Property{"index_of_refraction", val.index_of_refraction});
}

/**
 *@brief parse a cpp_raytracing::Dielectric from a node tree
 */
template <>
Dielectric parse_node(const tao::pegtl::parse_tree::node& node) {
    Color color = Colors::BLACK;
    Scalar index_of_refraction = 0.0;
    for (const auto& child : node.children) {
        if (child->is_type<grammar::mat_dielectric_color>()) {
            color = parse_node<Color>(*(child->children[0]));
        }
        if (child->is_type<grammar::mat_dielectric_ior>()) {
            index_of_refraction = parse_node<Scalar>(*(child->children[0]));
        }
    }
    // assert success
    return Dielectric{color, index_of_refraction};
}

}} // namespace cpp_raytracing::io

#endif
