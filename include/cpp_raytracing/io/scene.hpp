/**
 * @file
 * @brief Scene IO
 */

#ifndef CPP_RAYTRACING_IO_SCENE_HPP
#define CPP_RAYTRACING_IO_SCENE_HPP

#include "../scene.hpp"
#include "base.hpp"
#include "hittables.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct scene_prefix : pegtl::string<'S', 'c', 'e', 'n', 'e'> {};
struct scene_hittables : property<hittable, 'c', 'o', 'l', 'o', 'r'> {};
struct scene_properties : tuple<scene_hittables> {};
/** @brief grammar for cpp_raytracing::Scene */
struct scene : pegtl::seq<scene_prefix, pegtl::pad<scene_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Scene */
template <>
struct grammar_for<Scene> {
    /** @brief grammar to be selected */
    using get = scene;
};

/** @brief selector of cpp_raytracing::Scene for partial parse tree */
template <>
struct selector<scene> : store_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Scene to a stream */
template <>
void write<Scene>(std::ostream& os, const Scene& val) {
    os << "Scene ";
    write_tuple(os, Property{"camera", val.camera},
                Property{"hittables", val.hittables()});
}

/**
 *@brief parse a cpp_raytracing::Scene from a node tree
 */
template <>
Scene parse_node(const tao::pegtl::parse_tree::node& node) {}

}} // namespace cpp_raytracing::io

#endif
