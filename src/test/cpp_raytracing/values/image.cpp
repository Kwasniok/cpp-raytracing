#define BOOST_TEST_MODULE cpp_raytracing::render::image

#include "../../common.hpp"

#include <array>
#include <cmath>
#include <sstream>

#include <cpp_raytracing/values/image.hpp>

namespace but = boost::unit_test;
namespace ray = cpp_raytracing;

constexpr ray::Scalar epsilon = 1.0e-12;

BOOST_AUTO_TEST_CASE(constructor) {
    ray::Image2D(10, 10);
}

BOOST_AUTO_TEST_CASE(properties) {
    const ray::Image2D img{10, 20};
    BOOST_CHECK(img.width() == 10ul);
    BOOST_CHECK(img.height() == 20ul);
}

BOOST_AUTO_TEST_CASE(operator_bracket_const, *but::tolerance(epsilon)) {
    const ray::Color color{0.0, 0.5, 1.0};
    ray::Image2D image{10, 10};
    image[{1, 2}] = color;
    const ray::Image2D& cimg = image;

    TEST_EQUAL_RANGES(cimg[std::pair(1, 2)], color);
}

BOOST_AUTO_TEST_CASE(operator_bracket_mut, *but::tolerance(epsilon)) {
    const ray::Color color{0.0, 0.5, 1.0};
    ray::Image2D image{10, 10};
    image[{1, 2}] = color;

    TEST_EQUAL_RANGES(image[std::pair(1, 2)], color);
}

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct Image2DArithmeticFixture {

    Image2DArithmeticFixture() {
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
    ~Image2DArithmeticFixture() = default;

    const unsigned long N = 8;
    const ray::ColorScalar f = 8.0;
    ray::Image2D img1{N, N};
    ray::Image2D img2{N, N};
};

BOOST_FIXTURE_TEST_CASE(inplace_plus, Image2DArithmeticFixture,
                        *but::tolerance(epsilon)) {
    img1 += img2;
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            TEST_EQUAL_RANGES(img1[std::pair(i, j)],
                              ray::Color(N * N, N * N, 0.0));
        }
    }
}

BOOST_FIXTURE_TEST_CASE(inplace_multiply, Image2DArithmeticFixture,
                        *but::tolerance(epsilon)) {
    img2 *= f;
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            TEST_EQUAL_RANGES(img2[std::pair(i, j)],
                              ray::Color(f * i, f * j, 0.0));
        }
    }
}

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct ColorIOFixture {

    ColorIOFixture() = default;
    ~ColorIOFixture() = default;

    const ray::Color color = ray::Color{0.0, 128.0, 255.0} / 255.0;
    const std::string string = "0 128 255";
};

BOOST_FIXTURE_TEST_CASE(write_color_as_ascii_triple, ColorIOFixture) {
    std::stringstream ss;
    ray::write_color_as_ascii_triple(ss, color);
    TEST_EQUAL_RANGES(ss.str(), string);
}

BOOST_FIXTURE_TEST_CASE(read_color_from_ascii_triple, ColorIOFixture,
                        *but::tolerance(1e-2)) {
    std::stringstream ss(string);
    ray::Color c = ray::read_color_from_ascii_triple(ss);
    TEST_EQUAL_RANGES(c, color);
}

// NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
struct Image2DIOFixture {

    // note: Enforcing ASCII format correctness on character by character basis
    //       is in general not correct (disrespects the PPM grammar) but is most
    //       simple to implement.

    Image2DIOFixture() {
        for (unsigned long i = 0; i < N; ++i) {
            for (unsigned long j = 0; j < M; ++j) {
                image[{i, j}] = ray::Color(i * 1.0, j * 1.0, 0.0);
                image_from_ppm[{i, j}] =
                    ray::Color{i != 0 ? 1.0 : 0.0, j != 0 ? 1.0 : 0.0, 0.0};
            }
        }
    }
    ~Image2DIOFixture() = default;

    const unsigned long N = 2, M = 3;
    ray::Image2D image{N, M};
    // note: Writing to ppm format results in clipping, therefore the restored
    //       image differs.
    ray::Image2D image_from_ppm{N, M};
    const std::string string{"P3\n\
2 3\n\
255\n\
0 255 0   255 255 0   \n\
0 255 0   255 255 0   \n\
0 0 0   255 0 0   \n"};
    const std::array<std::uint8_t, 82> binary{
        0x50, 0x46, 0x0a, 0x32, 0x20, 0x33, 0x0a, 0x2d, 0x31, 0x0a, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x80, 0x3f, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00};
};

BOOST_FIXTURE_TEST_CASE(write_image_ppm, Image2DIOFixture) {
    std::stringstream ss;
    ray::write_image_ppm(ss, image);
    TEST_EQUAL_RANGES(ss.str(), string);
}

BOOST_FIXTURE_TEST_CASE(read_image_ppm, Image2DIOFixture,
                        *but::tolerance(epsilon)) {
    std::stringstream ss(string);
    ray::Image2D img = ray::read_image_ppm(ss);
    BOOST_CHECK(img.width() == image_from_ppm.width());
    BOOST_CHECK(img.height() == image_from_ppm.height());
    for (unsigned long i = 0; i < image_from_ppm.width(); i++) {
        for (unsigned long j = 0; j < image_from_ppm.height(); j++) {
            const auto& c = img[{i, j}];
            const auto& color = image_from_ppm[{i, j}];
            TEST_EQUAL_RANGES(c, color);
        }
    }
}

BOOST_FIXTURE_TEST_CASE(write_image_pfm, Image2DIOFixture,
                        *but::tolerance(epsilon)) {
    std::stringstream ss;
    ray::write_image_pfm(ss, image);
    const auto s = ss.str();
    BOOST_REQUIRE(s.size() == binary.size());
    for (std::size_t i = 0; i < binary.size(); ++i) {
        BOOST_CHECK(static_cast<std::uint8_t>(s.at(i)) == binary.at(i));
    }
}
