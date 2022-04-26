/**
 * @file
 * @brief unit test framework
 */

#ifndef TEST_CPP_RAYTRACING_TEST_HPP
#define TEST_CPP_RAYTRACING_TEST_HPP

#include <exception>
#include <iostream>
#include <sstream>
#include <string>

namespace cpp_raytracing {

/** @brief entry point for test suite */
void run_test_suite();

/** @brief signals assertion errors during unit tests */
struct AssertionFailedException : std::exception {

    /** @brief construct failed assertion exception from message */
    AssertionFailedException(std::string&& message) : message(message) {}
    /** @brief construct failed assertion exception from message */
    AssertionFailedException(const char* message) : message(message) {}

    /** @brief get message */
    virtual const char* what() const noexcept override {
        return message.c_str();
    }

  private:
    std::string message;
};

namespace internal {

/** @brief generates message of failed assertion */
inline std::string message(const char* expr, const char* file, const int line,
                           const char* reason) {

    std::stringstream msg;
    msg << "at " << file << ":" << line << std::endl;
    msg << "Expression `" << expr << "` " << reason << ".";
    return msg.str();
}

/**
 @brief asserts expression is true
 @note internal usage only
 @see TEST_ASSERT_TRUE
 */
template <typename T>
inline void assert_true(T&& x, const char* expr, const char* file,
                        const int line) {
    if (x) {
        // do nothing
    } else {
        throw AssertionFailedException(
            message(expr, file, line, "is not true"));
    }
}

/**
 @brief asserts expression is false
 @note internal usage only
 @see TEST_ASSERT_FALSE
 */
template <typename T>
inline void assert_false(T&& x, const char* expr, const char* file,
                         const int line) {
    if (x) {
        throw AssertionFailedException(
            message(expr, file, line, "is not false"));
    }
}

} // namespace internal

/**
 * @brief asserts expression is true
 */
#define TEST_ASSERT_TRUE(expr)                                                 \
    cpp_raytracing::internal::assert_true(expr, #expr, __FILE__, __LINE__)
/**
 * @brief asserts expression is false
 */
#define TEST_ASSERT_FALSE(expr)                                                \
    cpp_raytracing::internal::assert_false(expr, #expr, __FILE__, __LINE__)

} // namespace cpp_raytracing

/** @brief entry point for unit test program */
int main() {
    using namespace std;

    try {
        cpp_raytracing::run_test_suite();
    } catch (const cpp_raytracing::AssertionFailedException& e) {
        cout << "\033[1;31mfailed\033[0m" << endl;
        cout << "\033[1;31m" << e.what() << "\033[0m" << endl;
        return EXIT_FAILURE;
    }

    cout << "\033[1;32mpassed\033[0m" << endl;
    return EXIT_SUCCESS;
}

#endif
