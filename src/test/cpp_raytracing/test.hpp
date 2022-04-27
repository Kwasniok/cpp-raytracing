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

namespace cpp_raytracing { namespace test {

/** @brief signature of a test case function */
using test_case_t = void (*)(void);

/** @brief entry point for test suite */
void run_test_suite(void);

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
inline void assert_true(const T& x, const char* expr, const char* file,
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
inline void assert_false(const T& x, const char* expr, const char* file,
                         const int line) {
    if (x) {
        throw AssertionFailedException(
            message(expr, file, line, "is not false"));
    }
}


/** @brief indicates finishing a test case */
inline void indicate_finished_test_case() {
    std::cout << ".";
}

} // namespace internal

/**
 * @brief asserts expression is true
 */
#define TEST_ASSERT_TRUE(expr)                                                 \
    cpp_raytracing::test::internal::assert_true(expr, #expr, __FILE__, __LINE__)
/**
 * @brief asserts expression is false
 */
#define TEST_ASSERT_FALSE(expr)                                                \
    cpp_raytracing::test::internal::assert_false(expr, #expr, __FILE__,        \
                                                 __LINE__)
/**
 * @brief asserts expression is false
 */
#define TEST_ASSERT_IN_RANGE(min, max, expr)                                   \
    cpp_raytracing::test::internal::assert_in_range(min, max, expr, #expr,     \
                                                    __FILE__, __LINE__)
/**
 * @brief call a test case function
 */
inline void run(const test_case_t func) {
    func();
    cpp_raytracing::test::internal::indicate_finished_test_case();
}

}} // namespace cpp_raytracing::test

/** @brief entry point for unit test program */
int main() {
    using namespace std;

    try {
        cpp_raytracing::test::run_test_suite();
    } catch (const cpp_raytracing::test::AssertionFailedException& e) {
        cout << " \033[1;31mfailed\033[0m" << endl;
        cout << "\033[1;31m" << e.what() << "\033[0m" << endl;
        return EXIT_FAILURE;
    }

    cout << " \033[1;32mpassed\033[0m" << endl;
    return EXIT_SUCCESS;
}

#endif
