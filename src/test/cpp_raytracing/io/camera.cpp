#include <cpp_raytracing/io/camera.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<Camera, const char*>> data = {
        {
            Camera{
                .origin = {1, 2, 3},
                .direction_x = {1, 0, 0},
                .direction_y = {0, 1, 0},
                .direction_z = {0, 0, -1},
                .lens_radius = 0.7,
            },
            "Camera {"
            "origin = {1, 2, 3}, "
            "direction_x = {1, 0, 0}, "
            "direction_y = {0, 1, 0}, "
            "direction_z = {0, 0, -1}, "
            "lens_radius = 0.7"
            "}",
        },
    };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success() {
    const std::vector<std::pair<Camera, const char*>> data = {
        {
            Camera{
                .origin = {1, 2, 3},
                .direction_x = {1, 0, 0},
                .direction_y = {0, 1, 0},
                .direction_z = {0, 0, -1},
                .lens_radius = 0.7,
            },
            "Camera {"
            "origin = {1, 2, 3}, "
            "direction_x = {1, 0, 0}, "
            "direction_y = {0, 1, 0}, "
            "direction_z = {0, 0, -1}, "
            "lens_radius = 0.7"
            "}",
        },
        // variation
        {
            Camera{
                .origin = {1, 2, 3},
                .direction_x = {1, 0, 0},
                .direction_y = {0, 1, 0},
                .direction_z = {0, 0, -1},
                .lens_radius = 0.7,
            },
            "Camera { "
            "    origin = {1, 2, 3}, "
            "    direction_x = {1, 0, 0}, "
            "    direction_y = {0, 1, 0}, "
            "    direction_z = {0, 0, -1}, "
            "    lens_radius = 0.7, "
            "}",
        },
    };
    for (const auto& [val, str] : data) {
        Camera x = io::read<Camera>(str);
        TEST_ASSERT_EQUAL(x.origin, val.origin);
        TEST_ASSERT_EQUAL(x.direction_x, val.direction_x);
        TEST_ASSERT_EQUAL(x.direction_y, val.direction_y);
        TEST_ASSERT_EQUAL(x.direction_z, val.direction_z);
        TEST_ASSERT_EQUAL(x.lens_radius, val.lens_radius);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "Camera {{1, 2, 3}, {1, 0, 0}, {0, 1, 0}, {0, 0, -1}, 0.7}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Camera>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
