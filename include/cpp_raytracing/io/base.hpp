/**
 * @file
 * @brief base IO
 */

#ifndef CPP_RAYTRACING_IO_BASE_HPP
#define CPP_RAYTRACING_IO_BASE_HPP

#include <charconv>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
namespace pegtl = tao::pegtl;

namespace cpp_raytracing { namespace io {

/** @brief signals parsing errors */
struct ParsingException : std::exception {

    /** @brief construct parsing exception from message */
    ParsingException(std::string&& message) : message(message) {}
    /** @brief construct parsingexception from message */
    ParsingException(const char* message) : message(message) {}

    /** @brief get message */
    virtual const char* what() const noexcept override {
        return message.c_str();
    }

  private:
    std::string message;
};

namespace grammar {

/**
 * @brief selector for partial parse tree (default=false)
 * @tparam T value type
 * @see parse_node, read
 * @note Only selected parts of the grammar will be present in the parse tree.
 *       All other parts will be considered auxiliary and will be omitted.
 */
template <typename T>
struct selector : std::false_type {};

/**
 * @brief grammar for parsing an isolated value only
 * @tparam T value type
 * @see parse_node, read
 */
template <typename T>
struct grammar_for {
    /** @brief grammar to be selected */
    using get = void;
};

} // namespace grammar

/**
 * @brief write a value to a stream
 * @tparam T value type
 */
template <typename T>
void write(std::ostream& os, const T&);

/**
 * @brief parse a value from a node tree
 * @tparam T value type
 * @note asserts successful parsing of the tree
 * @note asserts node not to be the root of the tree
 */
template <typename T>
T parse_node(const tao::pegtl::parse_tree::node& node);

namespace {
inline void throw_read_exception(const pegtl::memory_input<>& in,
                                 const pegtl::parse_error& e) {
    const auto p = e.positions().front();
    std::stringstream msg;
    msg << "Parsing error: " << e.what() << std::endl
        << in.line_at(p) << '\n'
        << std::setw(p.column) << '^';
    throw ParsingException(msg.str());
}
} // namespace

/**
 * @brief read a value from memory
 * @tparam T value type
 * @tparam G grammar for value type
 * @see grammar::value, grammar::selector, parse_node
 */
template <typename T, typename G = grammar::grammar_for<T>::get>
T read(pegtl::memory_input<>& in) {
    try {
        // NOTE: pegtl::must guarantees either success or throw
        const auto root = pegtl::parse_tree::parse<pegtl::must<G, pegtl::eof>,
                                                   grammar::selector>(in);
        if (root) {
            return parse_node<T>(*(root->children[0]));
        }
        // this part is never reached ...
    } catch (const pegtl::parse_error& e) {
        throw_read_exception(in, e);
    }
    // this part is never reached ...
    // throw for completeness only
    throw std::runtime_error("Unexpected control flow.");
}

/**
 * @brief read a value from a string
 * @tparam T value type
 * @tparam G grammar for value type
 * @see grammar::value, grammar::selector, parse_node
 */
template <typename T, typename G = grammar::grammar_for<T>::get>
T read(const std::string& str) {
    pegtl::memory_input in(str, "");
    return read<T, G>(in);
}

namespace grammar {

/** @brief white space character */
struct ws_char : pegtl::one<' ', '\t', '\n'> {};
/** @brief white space */
struct ws : pegtl::plus<ws_char> {};

/** @brief signal beginning of enumerations and alike */
struct brace_open : pegtl::string<'{'> {};
/** @brief signal end of enumerations and alike */
struct brace_close : pegtl::string<'}'> {};
/** @brief separator for enumerations and alike */
struct separator : pegtl::string<','> {};
/** @brief quotation marks */
struct quote : pegtl::one<'"'> {};
/** @brief decimal dot */
struct decimal : pegtl::one<'.'> {};

} // namespace grammar
}} // namespace cpp_raytracing::io

#endif
