/**
 * @file
 * @brief Scalar IO
 */

#ifndef CPP_RAYTRACING_IO_SCALAR_HPP
#define CPP_RAYTRACING_IO_SCALAR_HPP

#include "../scalar.hpp"
#include "base.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

/** @brief grammar for ::Scalar */
struct scalar : grammar::floating {};

/** @brief grammar for parsing a ::Scalar */
template <>
struct grammar_for<Scalar> {
    /** @brief grammar to be selected */
    using get = scalar;
};

/** @brief selector of ::Scalar for partial parse tree */
template <>
struct selector<scalar> : std::true_type {};

} // namespace grammar

/** @brief write a ::Scalar to a stream */
template <>
void write<Scalar>(std::ostream& os, const Scalar& val) {
    if (-infinity < val && val < infinity) {
        std::string s = fmt::format("{}", val);
        os << s;
        return;
    }
    if (val == infinity) {
        os << "inf";
        return;
    }
    if (val == -infinity) {
        os << "-inf";
        return;
    }
    // for completeness
    os << "nan";
}

/**
 *@brief parse a ::Scalar from a node tree
 */
template <>
Scalar parse_node(const tao::pegtl::parse_tree::node& node) {
    Scalar scalar;
    const auto& str = node.string_view();
    if (str == "inf") {
        return infinity;
    }
    if (str == "+inf") {
        return infinity;
    }
    if (str == "-inf") {
        return -infinity;
    }
    std::from_chars(str.data(), str.data() + str.size(), scalar);
    // assert success
    // TODO: catch errors (e.g. too many digits)
    return scalar;
}

}} // namespace cpp_raytracing::io

#endif
