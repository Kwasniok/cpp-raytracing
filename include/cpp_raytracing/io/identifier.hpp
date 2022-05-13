/**
 * @file
 * @brief Identifier IO
 */

#ifndef CPP_RAYTRACING_IO_IDENTIFIER_HPP
#define CPP_RAYTRACING_IO_IDENTIFIER_HPP

#include <sstream>
#include <utility>

#include "../identifier.hpp"
#include "base.hpp"

namespace cpp_raytracing { namespace io {
namespace grammar {

struct identifier_content_char : pegtl::sor<pegtl::alnum, pegtl::one<'_'>> {};
/** @see Identifier::valid() */
struct identifier_content : pegtl::plus<identifier_content_char> {};
/** @brief grammar for cpp_raytracing::Identifier */
struct identifier : quoted<identifier_content> {};

/** @brief grammar for parsing a cpp_raytracing::Identifier */
template <typename T>
struct grammar_for<Identifier<T>> {
    /** @brief grammar to be selected */
    using get = identifier;
};

/** @brief selector of cpp_raytracing::Identifier for partial parse tree */
template <>
struct selector<identifier_content> : store_content {};

} // namespace grammar

/**
 * @brief dummy struct to bundle io operations in case of partial template
 *        specialization for Identifier
 * @see io::PartialSpecialDummy
 */
template <typename T>
struct PartialSpecialDummy<Identifier<T>> {
    /** @brief write a cpp_raytracing::Identifier to a stream */
    static void write(std::ostream& os, const Identifier<T>& val) {
        io::write(os, Quoted{val.str()});
    }
    /**
     *@brief parse a cpp_raytracing::Identifier from a node tree
     */
    static Identifier<T> parse_node(const tao::pegtl::parse_tree::node& node) {
        std::string value{node.string_view()};
        std::optional<Identifier<T>> opt_identifier =
            Identifier<T>::make_if_available(std::move(value));
        if (!opt_identifier) {
            std::stringstream msg;
            msg << "Cannot parse identifier `" << node.string_view()
                << "`. Identifier is either invalid or taken.";
            throw ParsingException(std::move(msg).str());
        }
        return std::move(opt_identifier).value();
    }
};

}} // namespace cpp_raytracing::io

#endif
