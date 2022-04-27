#include "test.hpp"

namespace cpp_raytracing { namespace test {

void test_example() {
    TEST_ASSERT_TRUE(true);
}

void run_test_suite() {
    run(test_example);
}

}} // namespace cpp_raytracing::test
