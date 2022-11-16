#define BOOST_TEST_MODULE cpp_raytracing::render::image

#include "../../common.hpp"

#include <array>
#include <sstream>

#include <cpp_raytracing/render/image.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(constructor) {
    ray::RawImage(10, 10);
}

BOOST_AUTO_TEST_CASE(properties) {
    const ray::RawImage img{10, 20};
    BOOST_CHECK(img.width() == 10ul);
    BOOST_CHECK(img.height() == 20ul);
}

BOOST_AUTO_TEST_CASE(operator_bracket_const, *but::tolerance(epsilon)) {
    const ray::Color color{0.0, 0.5, 1.0};
    ray::RawImage image{10, 10};
    image[{1, 2}] = color;
    const ray::RawImage& cimg = image;

    TEST_EQUAL_RANGES(cimg[std::pair(1, 2)], color);
}

BOOST_AUTO_TEST_CASE(operator_bracket_mut, *but::tolerance(epsilon)) {
    const ray::Color color{0.0, 0.5, 1.0};
    ray::RawImage image{10, 10};
    image[{1, 2}] = color;

    TEST_EQUAL_RANGES(image[std::pair(1, 2)], color);
}

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct RawImageArithmeticFixture {

    RawImageArithmeticFixture() {
        for (unsigned long j = 0; j < N; ++j) {
            for (unsigned long i = 0; i < N; ++i) {
                const auto a = static_cast<ray::ColorScalar>(N * N - i);
                const auto b = static_cast<ray::ColorScalar>(N * N - j);
                const auto c = static_cast<ray::ColorScalar>(i);
                const auto d = static_cast<ray::ColorScalar>(j);
                img1[{i, j}] = ray::Color(a, b, 0.0);
                img2[{i, j}] = ray::Color(c, d, 0.0);
            }
        }
    }
    ~RawImageArithmeticFixture() = default;

    const unsigned long N = 8;
    const ray::ColorScalar f = 8.0;
    ray::RawImage img1{N, N};
    ray::RawImage img2{N, N};
};

BOOST_FIXTURE_TEST_CASE(inplace_plus, RawImageArithmeticFixture,
                        *but::tolerance(epsilon)) {
    img1 += img2;
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            TEST_EQUAL_RANGES(img1[std::pair(i, j)],
                              ray::Color(N * N, N * N, 0.0));
        }
    }
}

BOOST_FIXTURE_TEST_CASE(inplace_multiply, RawImageArithmeticFixture,
                        *but::tolerance(epsilon)) {
    img2 *= f;
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            TEST_EQUAL_RANGES(img2[std::pair(i, j)],
                              ray::Color(f * i, f * j, 0.0));
        }
    }
}

BOOST_AUTO_TEST_CASE(write_image_ppm) {
    const unsigned long N = 2;
    const unsigned long M = 3;
    // note: Enforcing output correctness on character by character basis is in
    //       general not correct (disrespects the PPM grammar) but is most
    //       simple to implement.
    const std::string output{"P3 # ASCII 8-bit RGB\n\
2 3 # width x height\n\
255 # max color value per channel\n\
0 255 0   255 255 0   \n\
0 255 0   255 255 0   \n\
0 0 0   255 0 0   \n"};
    ray::RawImage img{N, M};
    for (unsigned long i = 0; i < N; ++i) {
        for (unsigned long j = 0; j < M; ++j) {
            const auto a = static_cast<ray::ColorScalar>(i);
            const auto b = static_cast<ray::ColorScalar>(j);
            img[{i, j}] = ray::Color(a, b, 0.0);
        }
    }

    {
        std::stringstream ss;
        ray::write_image_ppm(ss, img);
        BOOST_CHECK(ss.str() == output);
    }
}

BOOST_AUTO_TEST_CASE(write_image_pfm, *but::tolerance(epsilon)) {
    const unsigned long N = 2;
    const unsigned long M = 3;
    // note: Enforcing output correctness on character by character basis is in
    //       general not correct (disrespects the PFM grammar) but is most
    //       simple to implement.
    const std::array<std::uint8_t, 82> output{
        0x50, 0x46, 0x0a, 0x32, 0x20, 0x33, 0x0a, 0x2d, 0x31, 0x0a, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
    ray::RawImage img{N, M};
    for (unsigned long i = 0; i < N; ++i) {
        for (unsigned long j = 0; j < M; ++j) {
            const auto a = static_cast<ray::ColorScalar>(i);
            const auto b = static_cast<ray::ColorScalar>(j);
            img[{i, j}] = ray::Color(a, b, 0.0);
        }
    }

    {
        std::stringstream ss;
        ray::write_image_pfm(ss, img);
        const auto s = ss.str();
        BOOST_REQUIRE(s.size() == output.size());
        for (std::size_t i = 0; i < output.size(); ++i) {
            BOOST_CHECK(static_cast<std::uint8_t>(s.at(i)) == output.at(i));
        }
    }
}
