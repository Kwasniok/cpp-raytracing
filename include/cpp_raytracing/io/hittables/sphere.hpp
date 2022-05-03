/**
 * @file
 * @brief sphere IO
 */

#ifndef CPP_RAYTRACING_IO_HITTABLES_SPHERE_HPP
#define CPP_RAYTRACING_IO_HITTABLES_SPHERE_HPP

#include "../../hittables/sphere.hpp"
#include "../base.hpp"
#include "../scalar.hpp"
#include "../vec3.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct hit_sphere_prefix : pegtl::string<'S', 'p', 'h', 'e', 'r', 'e'> {};
struct hit_sphere_origin : property<vec3, 'o', 'r', 'i', 'g', 'i', 'n'> {};
struct hit_sphere_radius : property<scalar, 'r', 'a', 'd', 'i', 'u', 's'> {};
struct hit_sphere_properties : tuple<hit_sphere_origin, hit_sphere_radius> {
}; // TODO: relax order
/** @brief grammar for cpp_raytracing::Sphere */
struct hit_sphere
    : pegtl::seq<hit_sphere_prefix, pegtl::pad<hit_sphere_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Sphere */
template <>
struct grammar_for<Sphere> {
    /** @brief grammar to be selected */
    using get = hit_sphere;
};

/** @brief selector of cpp_raytracing::Sphere for partial parse tree */
template <>
struct selector<hit_sphere> : remove_content {};
template <>
struct selector<hit_sphere_origin> : remove_content {};
template <>
struct selector<hit_sphere_radius> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Sphere to a stream */
template <>
void write<Sphere>(std::ostream& os, const Sphere& val) {
    os << "Sphere ";
    write_tuple(os, Property{"origin", val.origin()},
                Property{"radius", val.radius()});
    // TODO: write material
}

/**
 *@brief parse a cpp_raytracing::Sphere from a node tree
 */
template <>
Sphere parse_node(const tao::pegtl::parse_tree::node& node) {
    Vec3 origin{0.0, 0.0, 0.0};
    Scalar radius = 0.0;
    std::shared_ptr<Material> material;

    for (const auto& child : node.children) {
        if (child->is_type<grammar::hit_sphere_origin>()) {
            origin = parse_node<Vec3>(*(child->children[0]));
        }
        if (child->is_type<grammar::hit_sphere_radius>()) {
            radius = parse_node<Scalar>(*(child->children[0]));
        }
        // TODO: parse material
    }

    return Sphere{origin, radius, std::move(material)};
}

}} // namespace cpp_raytracing::io

#endif
