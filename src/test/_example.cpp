#define BOOST_TEST_MODULE cpp_raytracing::example

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

namespace but = boost::unit_test;
namespace bdata = boost::unit_test::data;
namespace btt = boost::test_tools;

BOOST_AUTO_TEST_CASE(test) {
    BOOST_TEST(1 == 1);
}

BOOST_AUTO_TEST_CASE(warn) {
    BOOST_WARN(1 == 1);
}

BOOST_AUTO_TEST_CASE(require) {
    BOOST_REQUIRE(1 == 1);
}

BOOST_AUTO_TEST_CASE(floating_point, *but::tolerance(1e-5)) {
    BOOST_TEST(1.0 == 1.000001);
    BOOST_TEST(1.0 == 1.000001, btt::tolerance(1e-5));
}

constexpr std::array sample = {0, 1, 2, 3, 4};

BOOST_DATA_TEST_CASE(with_data, bdata::make(sample), x) {
    BOOST_TEST(x <= 4);
}

BOOST_DATA_TEST_CASE(with_zipped_data, bdata::make(sample) ^ sample, x, y) {
    BOOST_TEST(x == y);
}

BOOST_DATA_TEST_CASE(with_grid_data, bdata::xrange(2) * bdata::xrange(1), x,
                     y) {
    BOOST_TEST(x <= 4);
    BOOST_TEST(y <= 4);
}