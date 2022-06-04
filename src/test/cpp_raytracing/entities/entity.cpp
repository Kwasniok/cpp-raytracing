#include <cpp_raytracing/entities/entity.hpp>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

class Dummy : public Entity {};

void test_id() {
    Dummy dummy;
    dummy.id.change("test_id");
    TEST_ASSERT_EQUAL(dummy.id.str(), "test_id");
}

void run_test_suite() {
    run(test_id);
}

}} // namespace cpp_raytracing::test
