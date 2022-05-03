#include <cpp_raytracing/io/hittables/sphere.hpp>
#include <cpp_raytracing/materials.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    std::shared_ptr<Material> material =
        std::make_shared<Emitter>(Color{0.0, 0.5, 1.0});
    const std::vector<std::pair<Sphere, const char*>> data = {
        {
            Sphere{{1, 2, 3}, 0.7, material},
            "Sphere {"
            "origin = {1, 2, 3}, "
            "radius = 0.7"
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
    std::shared_ptr<Material> material =
        std::make_shared<Emitter>(Color{0.0, 0.5, 1.0});
    const std::vector<std::pair<Sphere, const char*>> data = {
        {
            Sphere{{1, 2, 3}, 0.7, material},
            "Sphere {"
            "origin = {1, 2, 3}, "
            "radius = 0.7"
            "}",
        },
    };
    for (const auto& [val, str] : data) {
        Sphere x = io::read<Sphere>(str);
        TEST_ASSERT_EQUAL(x.origin(), val.origin());
        TEST_ASSERT_EQUAL(x.radius(), val.radius());
        // TODO: material
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "Sphere {{1, 2, 3}, 0.7}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<Sphere>(str)), io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
