#include <cpp_raytracing/io/materials.hpp>
#include <sstream>
#include <utility>
#include <vector>

#include "../test.hpp"

namespace cpp_raytracing { namespace test {

void test_write() {
    const std::vector<std::pair<std::shared_ptr<Material>, const char*>> data =
        {
            {
                std::make_shared<Emitter>(Color{0.0, 0.5, 1}),
                "Emitter {color = {0, 0.5, 1}}",
            },
            {
                std::make_shared<Diffuse>(Color{0.0, 0.5, 1}),
                "Diffuse {color = {0, 0.5, 1}}",
            },
            {
                std::make_shared<Metal>(Color{0.0, 0.5, 1}, 0.7),
                "Metal {color = {0, 0.5, 1}, roughness = 0.7}",
            },
            {
                std::make_shared<Dielectric>(Color{0.0, 0.5, 1}, 0.7),
                "Dielectric {color = {0, 0.5, 1}, index_of_refraction = 0.7}",
            },
        };
    for (const auto& [val, str] : data) {
        std::stringstream os;
        io::write(os, val);
        TEST_ASSERT_EQUAL(os.str(), str);
    }
}

void test_read_success_emitter() {
    const std::vector<std::pair<std::shared_ptr<Emitter>, const char*>> data = {
        {
            std::make_shared<Emitter>(Color{0.0, 0.5, 1}),
            "Emitter {color = {0, 0.5, 1}}",
        },
    };
    for (const auto& [val, str] : data) {
        std::shared_ptr<Material> x = io::read<std::shared_ptr<Material>>(str);
        TEST_ASSERT_DYNAMIC_TYPE(*x.get(), Emitter);
        Emitter* mat = dynamic_cast<Emitter*>(x.get());
        TEST_ASSERT_EQUAL(mat->color, val->color);
    }
}

void test_read_success_diffuse() {
    const std::vector<std::pair<std::shared_ptr<Diffuse>, const char*>> data = {
        {
            std::make_shared<Diffuse>(Color{0.0, 0.5, 1}),
            "Diffuse {color = {0, 0.5, 1}}",
        },
    };
    for (const auto& [val, str] : data) {
        std::shared_ptr<Material> x = io::read<std::shared_ptr<Material>>(str);
        TEST_ASSERT_DYNAMIC_TYPE(*x.get(), Diffuse);
        Diffuse* mat = dynamic_cast<Diffuse*>(x.get());
        TEST_ASSERT_EQUAL(mat->color, val->color);
    }
}

void test_read_success_metal() {
    const std::vector<std::pair<std::shared_ptr<Metal>, const char*>> data = {
        {
            std::make_shared<Metal>(Color{0.0, 0.5, 1}, 0.7),
            "Metal {color = {0, 0.5, 1}, roughness = 0.7}",
        },
    };
    for (const auto& [val, str] : data) {
        std::shared_ptr<Material> x = io::read<std::shared_ptr<Material>>(str);
        TEST_ASSERT_DYNAMIC_TYPE(*x.get(), Metal);
        Metal* mat = dynamic_cast<Metal*>(x.get());
        TEST_ASSERT_EQUAL(mat->color, val->color);
        TEST_ASSERT_EQUAL(mat->roughness, val->roughness);
    }
}

void test_read_success_dielectric() {
    const std::vector<std::pair<std::shared_ptr<Dielectric>, const char*>>
        data = {
            {
                std::make_shared<Dielectric>(Color{0.0, 0.5, 1}, 0.7),
                "Dielectric {color = {0, 0.5, 1}, index_of_refraction = 0.7}",
            },
        };
    for (const auto& [val, str] : data) {
        std::shared_ptr<Material> x = io::read<std::shared_ptr<Material>>(str);
        TEST_ASSERT_DYNAMIC_TYPE(*x.get(), Dielectric);
        Dielectric* mat = dynamic_cast<Dielectric*>(x.get());
        TEST_ASSERT_EQUAL(mat->color, val->color);
        TEST_ASSERT_EQUAL(mat->index_of_refraction, val->index_of_refraction);
    }
}

void test_read_failure() {
    const std::vector<const char*> data = {
        "UnsopportedMaterial {color = {0, 0.5, 1}}",
    };
    for (const auto& str : data) {
        TEST_ASSERT_THROWS((io::read<std::shared_ptr<Material>>(str)),
                           io::ParsingException);
    }
}

void run_test_suite() {
    run(test_write);
    run(test_read_success_emitter);
    run(test_read_success_diffuse);
    run(test_read_success_metal);
    run(test_read_success_dielectric);
    run(test_read_failure);
}

}} // namespace cpp_raytracing::test
