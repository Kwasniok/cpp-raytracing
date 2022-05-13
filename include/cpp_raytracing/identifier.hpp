/**
 * @file
 * @brief identifier representation
 */

#ifndef CPP_RAYTRACING_IDENTIFIER_HPP
#define CPP_RAYTRACING_IDENTIFIER_HPP

#include <functional>
#include <istream>
#include <optional>
#include <set>
#include <string>
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
    std::set<hash_t> _storage;
};

} // namespace internal

/**
 * @brief represents an identifier for data of type T which are alphanumerical
 * strtings
 */
class Identifier {
  public:
    /** @brief move constructor */
    Identifier(Identifier&& other) = default;
    /** @brief move assignment */
    Identifier& operator=(Identifier&& other) = default;

    Identifier(const Identifier& other) = delete;
    Identifier& operator=(const Identifier& other) = delete;

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
     * @brief conditionally transforms string to identifier if it is not
     *        colliding
     * @returns identifer if it is not colliding with any other identifer else
     *          nothing
     */
    static std::optional<Identifier> make_if_available(std::string&& str) {
        if (!valid(str) || !_register.claim(str)) {
            return {};
        }
        return {Identifier{std::move(str)}};
    }
    /**
     * @brief unconditionally transforms string to identifier
     * @returns identifer which is either identical to the string (if it is
     *          non-colliding) or has a modified suffix (to avoid collisions)
     * @note In case the original string was not valid, an arbitrary valid root
     *       name is chosen.
     */
    static Identifier make_always(std::string&& str) {
        if (!valid(str)) {
            str = "identifier";
        }
        set_to_next_free(str);
        return {std::move(str)};
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
    Identifier(std::string&& str) : _value(std::move(str)) {}

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

internal::UniqueRegister Identifier::_register;

/** @brief write Identifier to ostream */
std::ostream& operator<<(std::ostream& os, const Identifier& identifier) {
    return os << identifier.str();
}

} // namespace cpp_raytracing

#endif
