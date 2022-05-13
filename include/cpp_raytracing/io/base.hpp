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
#include <string>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#ifdef DEBUG
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#endif
namespace pegtl = tao::pegtl;

namespace cpp_raytracing { namespace io {

/** @brief space character */
constexpr char SPACE = ' ';
/** @brief open brace character for listings and alike */
constexpr char BRACE_OPEN = '{';
/** @brief close brace character for listings and alike */
constexpr char BRACE_CLOSE = '}';
/** @brief separator character for listings and alike */
constexpr char SEPARATOR = ',';
/** @brief quotation character */
constexpr char QUOTE = '"';
/** @brief decimal point character for real numbers */
constexpr char DECIMAL = '.';
/** @brief assignment character */
constexpr char ASSIGN = '=';

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

/** @brief make node and store entire parsing string for it */
struct store_content : std::true_type {};

#ifdef DEBUG
/** @brief make node and store entire parsing string for it */
struct remove_content : std::true_type {};
#else
/**
 * @brief make node but do not store parsing string for it
 * @note does not affect children
 */
struct remove_content : pegtl::parse_tree::remove_content {};
#endif

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
 * @brief (specilazation) write an empty listing
 * @note: trailing comma is omitted
 * @see grammar::listing, ::SEPARATOR
 */
inline void write_listing(std::ostream& os) {}

/**
 * @brief (specilazation) write a listing of a single value
 * @tparam T value type
 * @note: trailing comma is omitted
 * @see grammar::listing, ::SEPARATOR
 */
template <typename T>
inline void write_listing(std::ostream& os, const T& val) {
    write(os, val);
}

/**
 * @brief write a listing of known size
 * @tparam Ts value types
 * @note: comma separated, trailing comma is omitted
 * @see grammar::listing, ::SEPARATOR
 */
template <typename T0, typename... Ts>
inline void write_listing(std::ostream& os, const T0& val0, const Ts&... vals) {
    write(os, val0);
    os << SEPARATOR << SPACE;
    write_listing<Ts...>(os, vals...);
}

/**
 * @brief write a brace-enclosed listing of known size
 * @tparam Ts value types
 * @see grammar::tuple, ::SEPARATOR, ::BRACE_OPEN, ::BRACE_CLOSE
 */
template <typename... Ts>
inline void write_tuple(std::ostream& os, const Ts&... vals) {
    os << BRACE_OPEN;
    write_listing<Ts...>(os, vals...);
    os << BRACE_CLOSE;
}

/**
 * @brief wrapper type to write a property
 * @see grammar::property
 * @note: This object is entended to be optimizd away by the compiler.
 */
template <typename T>
struct Property {
    /**
     * @brief wrapper type to write a property
     * @param name property name
     * @param value property value
     * @see grammar::property
     */
    constexpr explicit Property(const char* name, const T& value)
        : name(name), value(value){};

    /** @brief property name */
    const char* name;
    /** @brief property value */
    const T& value;
};

/**
 * @brief write a Property as an assignment
 * @tparam T value type
 * @see grammar::property
 */
template <typename T>
inline void write(std::ostream& os, const Property<T>& prop) {
    os << prop.name << ' ' << ASSIGN << ' ';
    write(os, prop.value);
}

/**
 * @brief wrapper type to write an object in quotes
 * @see grammar::quoted
 * @note: This object is entended to be optimizd away by the compiler.
 */
template <typename T>
struct Quoted {
    /**
     * @brief wrapper type to write an object in quotes
     * @see grammar::quoted
     * @note: This object is entended to be optimizd away by the compiler.
     */
    constexpr explicit Quoted(const T& value) : value(value){};

    /** @brief quoted value */
    const T& value;
};

/**
 * @brief write a quoted value
 * @tparam T value type
 * @see grammar::quoted
 */
template <typename T>
inline void write(std::ostream& os, const Quoted<T>& quoted) {
    os << QUOTE;
    write(os, quoted.value);
    os << QUOTE;
}

/**
 * @brief write a quoted string
 * @see grammar::quoted
 */
template <>
inline void write(std::ostream& os, const Quoted<std::string>& quoted) {
    os << QUOTE << quoted.value << QUOTE;
}

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
#ifdef DEBUG
            std::cout << "Obtained parsing tree while reading (DOT format):"
                      << std::endl;
            pegtl::parse_tree::print_dot(std::cout, *root);
#endif
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
struct ws_char : pegtl::one<SPACE, '\t', '\n'> {};
/** @brief white space */
struct ws : pegtl::plus<ws_char> {};

/** @brief signal beginning of enumerations and alike */
struct brace_open : pegtl::string<BRACE_OPEN> {};
/** @brief signal end of enumerations and alike */
struct brace_close : pegtl::string<BRACE_CLOSE> {};
/** @brief separator for enumerations and alike */
struct separator : pegtl::string<SEPARATOR> {};
/** @brief quotation marks */
struct quote : pegtl::one<QUOTE> {};
/** @brief decimal dot */
struct decimal : pegtl::one<DECIMAL> {};

namespace {
/**
 * @brief intercalate rules
 * @note @a Rs must end with void
 */
template <typename S, typename R0, typename... Rs>
struct intercalate : pegtl::seq<R0, S, intercalate<S, Rs...>> {};

template <typename S, typename R0>
struct intercalate<S, R0, void> : pegtl::seq<R0, pegtl::opt<S>> {};

template <typename S>
struct intercalate<S, void> : pegtl::seq<> {};
} // namespace

/**
 * @brief intercalate rules @a Rs with whitespace padded separators
 * @note equivalent to `pegtl::seq<R0, S, R1, S, ..., S, Rn, pegtl::opt<S>>`
 */
template <typename... Rs>
struct listing : intercalate<pegtl::pad<separator, ws>, Rs..., void> {};

/**
 * @brief brace enclosed tuple
 * @note e.g. `tuple<digit, digit>` matches `{1, 2}` and `{ 1, 2, }` etc.
 */
template <typename... Rs>
struct tuple
    : pegtl::seq<brace_open, pegtl::pad<listing<Rs...>, ws>, brace_close> {};

/**
 * @brief C-like static identifier with assignment
 * @note mind the reversed order or template parameters du to variadics
 */
template <typename T, char... Cs>
struct property
    : pegtl::seq<pegtl::string<Cs...>, pegtl::pad<pegtl::one<ASSIGN>, ws>, T> {
};

/**
 * @brief quoted value
 * @tparam R rule for quoted value
 * @note Do NOT allow ::QUOTE to appear inside of @a R!
 */
template <typename R>
struct quoted : pegtl::seq<quote, R, quote> {};

/** @brief single digit */
struct digit : pegtl::digit {};
/** @brief multiple digits */
struct digits : pegtl::plus<digit> {};
/** @brief sign of a number */
struct sign : pegtl::one<'+', '-'> {};

/** @brief mantissa of a real number */
struct mantissa
    : pegtl::seq<pegtl::opt<sign>, digits, pegtl::opt<decimal, digits>> {};
/** @brief exponent of a real number */
struct exponent : pegtl::seq<pegtl::one<'e'>, pegtl::opt<sign>, digits> {};
/** @brief real number */
struct real : pegtl::seq<mantissa, pegtl::opt<exponent>> {};

// NOTE: no NaN
/** @brief infinity symbol */
struct floating_infinity : pegtl::string<'i', 'n', 'f'> {};
/** @brief non-numerical values of a floating point number */
struct floating_special : pegtl::seq<pegtl::opt<sign>, floating_infinity> {};
/** @brief floating point number */
struct floating : pegtl::sor<real, floating_special> {};
} // namespace grammar
}} // namespace cpp_raytracing::io

#endif
