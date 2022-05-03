/**
 * @file
 * @brief Material IO
 */

#ifndef CPP_RAYTRACING_IO_MATERIALS_HPP
#define CPP_RAYTRACING_IO_MATERIALS_HPP

#include <memory>
#include <typeinfo>

#include "../materials.hpp"
#include "base.hpp"
#include "materials/dielectric.hpp"
#include "materials/diffuse.hpp"
#include "materials/emitter.hpp"
#include "materials/metal.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

/** @brief grammar for cpp_raytracing::Material */
struct material
    : pegtl::sor<mat_emitter, mat_diffuse, mat_metal, mat_dielectric> {};

/** @brief grammar for parsing a cpp_raytracing::Material */
template <>
struct grammar_for<std::shared_ptr<Material>> {
    /** @brief grammar to be selected */
    using get = material;
};

} // namespace grammar

namespace {

/** @brief downcast from shared pointer and write to stream */
template <typename D, typename B>
inline void write_cast(std::ostream& os, const std::shared_ptr<B>& val) {
    static_assert(std::is_base_of<B, D>::value, "D must be derived class of D");
    write<D>(os, dynamic_cast<const D&>(*val.get()));
}

} // namespace

/** @brief write a cpp_raytracing::Material to a stream */
template <>
void write<std::shared_ptr<Material>>(std::ostream& os,
                                      const std::shared_ptr<Material>& val) {
    // NOTE: This is not the object orientated approach!
    //       But otherwise inheritance from an abstract IO interface would be
    //       needed for all(!) data types. In addition this would also couple
    //       data types and io. (Currently IO depends on data types and not vice
    //       versa.)
    if (typeid(*val) == typeid(Emitter)) {
        write_cast<Emitter>(os, val);
    }
    if (typeid(*val) == typeid(Diffuse)) {
        write_cast<Diffuse>(os, val);
    }
    if (typeid(*val) == typeid(Metal)) {
        write_cast<Metal>(os, val);
    }
    if (typeid(*val) == typeid(Dielectric)) {
        write_cast<Dielectric>(os, val);
    }
}

/**
 *@brief parse a cpp_raytracing::Material from a node tree
 */
template <>
std::shared_ptr<Material> parse_node(const tao::pegtl::parse_tree::node& node) {
    if (node.is_type<grammar::mat_emitter>()) {
        return std::make_shared<Emitter>(parse_node<Emitter>(node));
    }
    if (node.is_type<grammar::mat_diffuse>()) {
        return std::make_shared<Diffuse>(parse_node<Diffuse>(node));
    }
    if (node.is_type<grammar::mat_metal>()) {
        return std::make_shared<Metal>(parse_node<Metal>(node));
    }
    if (node.is_type<grammar::mat_dielectric>()) {
        return std::make_shared<Dielectric>(parse_node<Dielectric>(node));
    }

    // this part is never reached ...
    throw ParsingException("Parse material implementation incomplete.");
}

}} // namespace cpp_raytracing::io

#endif
