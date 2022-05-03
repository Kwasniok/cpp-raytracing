/**
 * @file
 * @brief Camera IO
 */

#ifndef CPP_RAYTRACING_IO_CAMERA_HPP
#define CPP_RAYTRACING_IO_CAMERA_HPP

#include "../camera.hpp"
#include "base.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct camera_prefix : pegtl::string<'C', 'a', 'm', 'e', 'r', 'a'> {};
struct camera_origin : property<vec3, 'o', 'r', 'i', 'g', 'i', 'n'> {};
struct camera_direction_x
    : property<vec3, 'd', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', '_', 'x'> {};
struct camera_direction_y
    : property<vec3, 'd', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', '_', 'y'> {};
struct camera_direction_z
    : property<vec3, 'd', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', '_', 'z'> {};
struct camera_lens_radius
    : property<scalar, 'l', 'e', 'n', 's', '_', 'r', 'a', 'd', 'i', 'u', 's'> {
};
struct camera_properties
    : tuple<camera_origin, camera_direction_x, camera_direction_y,
            camera_direction_z, camera_lens_radius> {}; // TODO: relax order
/** @brief grammar for cpp_raytracing::Camera */
struct camera : pegtl::seq<camera_prefix, pegtl::pad<camera_properties, ws>> {};

/** @brief grammar for parsing a cpp_raytracing::Camera */
template <>
struct grammar_for<Camera> {
    /** @brief grammar to be selected */
    using get = camera;
};

/** @brief selector of cpp_raytracing::Camera for partial parse tree */
template <>
struct selector<camera> : remove_content {};
template <>
struct selector<camera_origin> : remove_content {};
template <>
struct selector<camera_direction_x> : remove_content {};
template <>
struct selector<camera_direction_y> : remove_content {};
template <>
struct selector<camera_direction_z> : remove_content {};
template <>
struct selector<camera_lens_radius> : remove_content {};

} // namespace grammar

/** @brief write a cpp_raytracing::Camera to a stream */
template <>
void write<Camera>(std::ostream& os, const Camera& val) {
    os << "Camera ";
    write_tuple(os, Property{"origin", val.origin},
                Property{"direction_x", val.direction_x},
                Property{"direction_y", val.direction_y},
                Property{"direction_z", val.direction_z},
                Property{"lens_radius", val.lens_radius});
}

/**
 *@brief parse a cpp_raytracing::Camera from a node tree
 */
template <>
Camera parse_node(const tao::pegtl::parse_tree::node& node) {
    Vec3 origin{0.0, 0.0, 0.0};
    Vec3 direction_x{1.0, 0.0, 0.0};
    Vec3 direction_y{0.0, 1.0, 0.0};
    Vec3 direction_z{0.0, 0.0, -1.0};
    Scalar lens_radius = 0.0;

    for (const auto& child : node.children) {
        if (child->is_type<grammar::camera_origin>()) {
            origin = parse_node<Vec3>(*(child->children[0]));
        }
        if (child->is_type<grammar::camera_direction_x>()) {
            direction_x = parse_node<Vec3>(*(child->children[0]));
        }
        if (child->is_type<grammar::camera_direction_y>()) {
            direction_y = parse_node<Vec3>(*(child->children[0]));
        }
        if (child->is_type<grammar::camera_direction_z>()) {
            direction_z = parse_node<Vec3>(*(child->children[0]));
        }
        if (child->is_type<grammar::camera_lens_radius>()) {
            lens_radius = parse_node<Scalar>(*(child->children[0]));
        }
    }

    return Camera{
        .origin = origin,
        .direction_x = direction_x,
        .direction_y = direction_y,
        .direction_z = direction_z,
        .lens_radius = lens_radius,
    };
}

}} // namespace cpp_raytracing::io

#endif
