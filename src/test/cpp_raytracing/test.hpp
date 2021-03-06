/**
 * @file
 * @brief unit test framework
 */

#ifndef TEST_CPP_RAYTRACING_TEST_HPP
#define TEST_CPP_RAYTRACING_TEST_HPP

#include <array>
#include <exception>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <string>
#include <typeinfo>
#include <utility>

#include <cpp_raytracing/util.hpp>

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
    return std::move(msg).str();
}

/** @brief generates message of failed assertion */
inline std::string message(const char* expr, const char* file, const int line,
                           const std::string& reason) {

    std::stringstream msg;
    msg << "at " << file << ":" << line << std::endl;
    msg << "Expression `" << expr << "` " << reason << ".";
    return std::move(msg).str();
}

/**
 * @brief throws an AssertionFailedException to indicate a non-thrown expected
 * exception
 */
inline std::string missed_throw(const char* expr, const char* file,
                                const int line, const char* exception) {

    std::stringstream msg;
    msg << "did not throw " << exception;
    throw AssertionFailedException(
        message(expr, file, line, std::move(msg).str()));
}

/**
 @brief asserts OpemMP is running with multiple threads
 @note internal usage only
 @see TEST_ASSERT_OPENMP_IS_MULTITHREADING
 */
inline void assert_openmp_is_multithreading(const char* file, const int line) {
    constexpr int N = 4;
    std::array<int, N> buffer{};
#pragma omp parallel for num_threads(N)
    for (int i = 0; i < N; ++i) {
        buffer[i] = omp_get_thread_num();
    }
    for (int i = 0; i < N; ++i) {
        if (buffer[i] != i) {
            throw AssertionFailedException(
                message("assert_openmp_is_multithreading()", file, line,
                        "failed because multithreading in OpenMP is not "
                        "(properly) enabled. Set OMP_THREAD_LIMIT>=4."));
        }
    }
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

/**
 @brief asserts expression is equal to value
 @note internal usage only
 @see TEST_ASSERT_EQUAL
 */
template <typename T, typename U>
inline void assert_equal(const T& x, const U& y, const char* expr,
                         const char* file, const int line) {
    if (x == y) {
        // do nothing
    } else {
        std::stringstream msg;
        msg << "= " << x << " is not equal to " << y;
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/**
 @brief asserts expression is not equal to value
 @note internal usage only
 @see TEST_ASSERT_NOT_EQUAL
 */
template <typename T, typename U>
inline void assert_not_equal(const T& x, const U& y, const char* expr,
                             const char* file, const int line) {
    if (x != y) {
        // do nothing
    } else {
        std::stringstream msg;
        msg << "= " << x << " is equal to " << y;
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/**
 @brief asserts expression is almost equal to value
 @note internal usage only
 @see TEST_ASSERT_ALMOST_EQUAL
 */
template <typename T, typename U, typename V>
inline void assert_almost_equal(const T& x, const U& y, const V& epsilon,
                                const char* expr, const char* file,
                                const int line) {
    if (std::abs(x - y) < epsilon) {
        // do nothing
    } else {
        std::stringstream msg;
        msg << "= " << x << " is not almost equal to " << y
            << " with precision of epsilon = " << epsilon;
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/**
 @brief asserts expression is not almost equal to value
 @note internal usage only
 @see TEST_ASSERT_NOT_ALMOST_EQUAL
 */
template <typename T, typename U, typename V>
inline void assert_not_almost_equal(const T& x, const U& y, const V& epsilon,
                                    const char* expr, const char* file,
                                    const int line) {
    if (std::abs(x - y) < epsilon) {
        std::stringstream msg;
        msg << "= " << x << " is almost equal to " << y
            << " with precision of epsilon = " << epsilon;
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/**
 @brief asserts iterable expression is almost equal to iterable value
 @note internal usage only
 @note complexity: O(n) = n^2
 @see TEST_ASSERT_ALMOST_EQUAL_ITERABLE
 */
template <typename T, typename U, typename V>
inline void assert_almost_equal_iterable(const T& x, const U& y,
                                         const V& epsilon, const char* expr,
                                         const char* file, const int line) {
    auto x_size = iterable_size(x);
    auto y_size = iterable_size(y);
    if (x_size != y_size) {
        std::stringstream msg;
        msg << " has size = " << x_size << " != " << y_size;
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }

    for (auto [i, a] : enumerate(x)) {
        for (auto [j, b] : enumerate(y)) {
            if (i == j) {
                if (std::abs(a - b) < epsilon) {
                    // do nothing
                } else {
                    std::stringstream msg;
                    msg << "@" << i << " = " << a << " is not almost equal to "
                        << b << " with precision of epsilon = " << epsilon;
                    throw AssertionFailedException(
                        message(expr, file, line, std::move(msg).str()));
                }
            }
        }
    }
}
/**
 @brief asserts iterable expression is not almost equal to iterable value
 @note internal usage only
 @see TEST_ASSERT_NOT_ALMOST_EQUAL_ITERABLE
 */
template <typename T, typename U, typename V>
inline void assert_not_almost_equal_iterable(const T& x, const U& y,
                                             const V& epsilon, const char* expr,
                                             const char* file, const int line) {
    if (iterable_size(x) != iterable_size(y)) {
        return;
    }

    for (auto [i, a] : enumerate(x)) {
        for (auto [j, b] : enumerate(y)) {
            if (i == j) {
                if (std::abs(a - b) < epsilon) {
                    // do nothing
                } else {
                    return;
                }
            }
        }
    }
    std::stringstream msg;
    msg << " = " << x << " is almost equal to " << y
        << " with precision of epsilon = " << epsilon;
    throw AssertionFailedException(
        message(expr, file, line, std::move(msg).str()));
}

/**
 @brief asserts expression is in a range (includes boundaries)
 @note internal usage only
 @see TEST_ASSERT_IN_RANGE
 */
template <typename T>
inline void assert_in_range(const T& x, const T& min, const T& max,
                            const char* expr, const char* file,
                            const int line) {
    if (x < min || x > max) {
        std::stringstream msg;
        msg << "= " << x << " is not in range[" << min << ", " << max << "] ";
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/**
 @brief asserts expression is a container with pairwise unique elements
 @note internal usage only
 @see TEST_ASSERT_PAIRWISE_UNIQUE
 */
template <typename T>
inline void assert_pairwise_unique(const T& x, const char* expr,
                                   const char* file, const int line) {
    for (auto [i, a] : enumerate(x)) {
        for (auto [j, b] : enumerate(x)) {
            if (i != j && a == b) {
                std::stringstream msg;
                msg << "has equivalent elements " << a << " (@" << i << ") and "
                    << b << " (@" << j << ")";
                throw AssertionFailedException(
                    message(expr, file, line, std::move(msg).str()));
            }
        }
    }
}

/**
 * @brief asserts expr has a specific dynamic type
 * @see TEST_ASSERT_DYNAMIC_TYPE
 */
inline void assert_dynamic_type(const std::type_info& x,
                                const std::type_info& y, const char* expr,
                                const char* file, const int line) {
    if (x != y) {
        std::stringstream msg;
        msg << "is of type " << x.name() << " not of type " << y.name();
        throw AssertionFailedException(
            message(expr, file, line, std::move(msg).str()));
    }
}

/** @brief indicates finishing a test case */
inline void indicate_finished_test_case() {
    std::cout << ".";
}

} // namespace internal

/**
 @brief asserts OpemMP is running with multiple threads
 */
#define TEST_ASSERT_OPENMP_IS_MULTITHREADING()                                 \
    cpp_raytracing::test::internal::assert_openmp_is_multithreading(__FILE__,  \
                                                                    __LINE__)
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
 * @brief asserts expression is equal to value
 */
#define TEST_ASSERT_EQUAL(expr, value)                                         \
    cpp_raytracing::test::internal::assert_equal(expr, value, #expr, __FILE__, \
                                                 __LINE__)
/**
 * @brief asserts expression is not equal to value
 */
#define TEST_ASSERT_NOT_EQUAL(expr, value)                                     \
    cpp_raytracing::test::internal::assert_not_equal(expr, value, #expr,       \
                                                     __FILE__, __LINE__)
/**
 * @brief asserts expression is almost equal to value
 */
#define TEST_ASSERT_ALMOST_EQUAL(expr, value, epsilon)                         \
    cpp_raytracing::test::internal::assert_almost_equal(                       \
        expr, value, epsilon, #expr, __FILE__, __LINE__)
/**
 * @brief asserts expression is not almost equal to value
 */
#define TEST_ASSERT_NOT_ALMOST_EQUAL(expr, value, epsilon)                     \
    cpp_raytracing::test::internal::assert_not_almost_equal(                   \
        expr, value, epsilon, #expr, __FILE__, __LINE__)

/**
 * @brief asserts iterable expression is almost equal to iterable value
 */
#define TEST_ASSERT_ALMOST_EQUAL_ITERABLE(expr, value, epsilon)                \
    cpp_raytracing::test::internal::assert_almost_equal_iterable(              \
        expr, value, epsilon, #expr, __FILE__, __LINE__)

/**
 * @brief asserts iterable expression is not almost equal to iterable value
 */
#define TEST_ASSERT_NOT_ALMOST_EQUAL_ITERABLE(expr, value, epsilon)            \
    cpp_raytracing::test::internal::assert_not_almost_equal_iterable(          \
        expr, value, epsilon, #expr, __FILE__, __LINE__)

/**
 * @brief asserts expression is false
 */
#define TEST_ASSERT_IN_RANGE(expr, min, max)                                   \
    cpp_raytracing::test::internal::assert_in_range(expr, min, max, #expr,     \
                                                    __FILE__, __LINE__)
/**
 * @brief asserts expression is a container with pairwise unique elements
 */
#define TEST_ASSERT_PAIRWISE_UNIQUE(expr)                                      \
    cpp_raytracing::test::internal::assert_pairwise_unique(expr, #expr,        \
                                                           __FILE__, __LINE__)
/**
 * @brief asserts call results in exception of given type
 * @note enclose @a call in parenthesis when using `<>` or `{}` to avoid
 * number of arguments missmatch error from preprocessor
 */
#define TEST_ASSERT_THROWS(call, exception)                                    \
    try {                                                                      \
        call;                                                                  \
        cpp_raytracing::test::internal::missed_throw(#call, __FILE__,          \
                                                     __LINE__, #exception);    \
    } catch (const exception& e) {                                             \
    }
/**
 * @brief asserts expr has dynamic type T (same typeid)
 */
#define TEST_ASSERT_DYNAMIC_TYPE(expr, T)                                      \
    cpp_raytracing::test::internal::assert_dynamic_type(                       \
        typeid(expr), typeid(T), #expr, __FILE__, __LINE__);

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
