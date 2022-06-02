/**
 * @file
 * @brief identifier representation
 */

#ifndef CPP_RAYTRACING_IDENTIFIER_HPP
#define CPP_RAYTRACING_IDENTIFIER_HPP

#include <functional>
#include <istream>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>

namespace cpp_raytracing {

namespace internal {

/**
 * @brief helper class to keep track of occupied strings
 * @note For efficiency, strings are hashed and only the hash is stored.
 */
class UniqueRegister {
  private:
    using hash_t = std::size_t;

  public:
    /**
     * @brief mark string as occupied
     * @returns true if occupation was successfull or false if it was allready
     *          occupied
     */
    bool claim(const std::string& str) {
        const auto [_, success] = _storage.insert(hash(str));
        if (success) {
        }
        return success;
    }
    /**
     * @brief mark string as available
     */
    void release(const std::string& str) { _storage.erase(hash(str)); }

  private:
    hash_t hash(const std::string& str) {
        return std::hash<std::string>{}(str);
    }

  private:
    std::unordered_set<hash_t> _storage;
};

} // namespace internal

/**
 * @brief helper class to determine the default identifier for a type
 * @see Identifer
 */
template <typename T>
struct default_identifier {
    /** @brief generic fallback value of the default identifer */
    static constexpr const char* value = "identifier";
};

/**
 * @brief represents an identifier for data of type T which are alphanumerical
 * @tparam identifies the collection of objects to be identified
 * @note Each object collection has its own identifier pool. Therefore, two
 *       identifiers of equal value but from different collections can be
 *       considered unequal.
 * @note Identifers are 'slippery' which means they might change the the suffix
 *       of their value in order to avoid collissions when created or changed.
 *       Use make_always if this is undesired.
 */
template <typename T>
class Identifier {
  private:
    /** @brief dummy struct to distinguish internal only overloads */
    struct InternalOnly {};

  public:
    /**
     * @brief initialize identifer with root based on default_identifier
     * @note Use this initializer if you don't care about the value.
     * @note Since a unoccupied identifier must be found, this initialization
     *       **might be costly**. Consider using Identifier::make_if_available
     *       or Identifier::make_always with a specific string if possible.
     *       (Move the value if necessary.)
     * @see make_if_available, make_always
     */
    Identifier() : _value() {
        std::string str = default_identifier<T>::value;
        set_to_next_free(str);
        _value = std::move(str);
    }

    /**
     * @brief move constructor
     * @note Moving preserves the identity.
     */
    Identifier(Identifier&& other) = default;

    /**
     * @brief move assignment
     * @note Moving preserves the identity.
     */
    Identifier& operator=(Identifier&& other) = default;

    /**
     * @brief non-copiable use explicit copy() instead
     * @note Copying would require to generate a new (incemented) identifer,
     *       this is non-trivial and should not happen silently.
     * @see copy
     */
    Identifier(const Identifier& other) = delete;

    /**
     * @brief non-copiable use explicit copy() instead
     * @note Copying would require to generate a new (incemented) identifer,
     *       this is non-trivial and should not happen silently.
     * @see copy
     */
    Identifier& operator=(const Identifier& other) = delete;

    /**
     * @brief initialize identifer with root based on parameter
     * @note Use this initializer if you care about the root of the value but
     *       not its exact value.
     * @note Since a unoccupied identifier must be found, this initialization
     *       **might be costly**. Consider using Identifier::make_if_available
     *       or Identifier::make_always with a specific string if possible.
     *       (Move the value if necessary.)
     * @see make_if_available, make_always
     */
    Identifier(std::string&& str) : _value() {
        set_to_next_free(str);
        _value = std::move(str);
    }

    /**
     * @brief conditionally transforms string to identifier if it is not
     *        colliding
     * @returns identifer if it is not colliding with any other identifer else
     *          nothing
     * @note Use this 'initializer' if you want to determine the exact value of
     *       the identifier.
     */
    static std::optional<Identifier> make_if_available(std::string&& str) {
        if (!valid(str) || !_register.claim(str)) {
            return {};
        }
        return {Identifier{std::move(str), InternalOnly()}};
    }
    /**
     * @brief unconditionally transforms string to identifier
     * @returns identifer which is either identical to the string (if it is
     *          non-colliding) or has a modified suffix (to avoid collisions)
     * @note Use this 'initializer' if you want to determine at least part of
     *       the value of the identifier.
     * @note In case the original string was not valid, a value based on the
     *       default identifier is generated.
     */
    static Identifier make_always(std::string&& str) {
        if (!valid(str)) {
            str = default_identifier<T>::value;
        }
        set_to_next_free(str);
        return {std::move(str), InternalOnly()};
    }

    ~Identifier() {
        if (has_value()) {
            _register.release(_value);
        }
    };

    /** @brief get string value */
    const std::string& str() const { return _value; }
    /** @brief get c-string value */
    const char* c_str() const { return _value.c_str(); }

    /** @brief equals comparison */
    bool operator==(const Identifier& other) const {
        return _value == other._value;
    }
    /** @brief equals comparison */
    bool operator==(const char* other) const { return _value == other; }
    /** @brief unequals comparison */
    bool operator!=(const Identifier& other) const {
        return _value != other._value;
    }
    /** @brief unequals comparison */
    bool operator!=(const char* other) const { return _value != other; }

    /**
     * @brief explicitly copies the identifer and increments it to a
     *        unoccupied value
     */
    Identifier copy() {
        std::string str = _value;
        set_to_next_free(str);
        return {std::move(str), InternalOnly()};
    }

    /**
     * @brief tests if string is valid
     * @note valid means it matches the regular expression `[a-zA-Z0-9_]+`
     */
    static bool valid(const std::string& str) {
        // NOTE: valid strings must never be empty to allow of optimized
        //       deconstruction (_value.size() == 0 <-> no value stored)
        // NOTE: valid strings must never contain quotation marks
        //       (") to allow  for simple IO
        return str.size() > 0 &&
               std::all_of(str.begin(), str.end(),
                           [](char c) { return std::isalnum(c) || c == '_'; });
    }

  private:
    /**
     * @brief initialize with std::string
     */
    Identifier(std::string&& str, InternalOnly) : _value(std::move(str)) {}

    /** @brief true if object has a value (which is then also registered) */
    bool has_value() const { return _value.size() > 0; }

    /**
     * @biref increment suffix in place
     * @param str must be Identifier::valid()
     * @note The end of @a str will be modified by one incremental step, e.g. to
     *       avoid an identifier collission.
     * @note The new value will be valid, differ from the original and but might
     *       have a collision.
     * @note Multiple consecutive applications will generate identifiers not
     *       generated before (= iteration).
     */
    static void set_to_next(std::string& str) {
        // TODO: IMPROVE naming scheme (mind memory usage)
        str += "__2";
    }
    /**
     * @biref update string in place to next possible free identifer
     * @param str must be Identifier::valid()
     * @note The end of @a str will be modified to avoid identifier collissions.
     * @note The new value will have no identifer collision.
     */
    static void set_to_next_free(std::string& str) {
        if (!_register.claim(str)) {
            set_to_next(str);
        }
    }

  private:
    /** @brief register for used strings */
    static internal::UniqueRegister _register;
    /** @brief identifer value */
    std::string _value; // NOTE: do not make const, must be movable
};

template <typename T>
internal::UniqueRegister Identifier<T>::_register;

/** @brief write Identifier to ostream */
template <typename T>
std::ostream& operator<<(std::ostream& os, const Identifier<T>& identifier) {
    return os << identifier.str();
}

} // namespace cpp_raytracing

#endif
