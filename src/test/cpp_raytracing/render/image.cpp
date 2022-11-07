#include "../../common.hpp"

#include <array>
#include <sstream>

#include <cpp_raytracing/render/image.hpp>

namespace cpp_raytracing { namespace test {

constexpr Scalar epsilon = 1.0e-12;

TEST_CASE("constructor") {
    RawImage(10, 10);
}

TEST_CASE("properties") {
    const RawImage img{10, 20};
    CHECK(img.width() == 10ul);
    CHECK(img.height() == 20ul);
}

TEST_CASE("operator_bracket") {
    const Color color{0.0, 0.5, 1.0};
    RawImage image{10, 10};
    image[{1, 2}] = color;

    SUBCASE("mutable") {
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, image[std::pair(1, 2)], color);
    }

    SUBCASE("const") {
        const RawImage& cimg = image;
        CHECK_ITERABLE_APPROX_EQUAL(epsilon, cimg[std::pair(1, 2)], color);
    }
}

TEST_CASE("arithmetic") {
    const unsigned long N = 8;
    const ColorScalar f = 8.0;
    RawImage img1{N, N};
    RawImage img2{N, N};
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            const auto a = static_cast<ColorScalar>(N * N - i);
            const auto b = static_cast<ColorScalar>(N * N - j);
            const auto c = static_cast<ColorScalar>(i);
            const auto d = static_cast<ColorScalar>(j);
            img1[{i, j}] = Color(a, b, 0.0);
            img2[{i, j}] = Color(c, d, 0.0);
        }
    }

    SUBCASE("add") {
        img1 += img2;
        for (unsigned long j = 0; j < N; ++j) {
            for (unsigned long i = 0; i < N; ++i) {
                CHECK_ITERABLE_APPROX_EQUAL(epsilon, img1[std::pair(i, j)],
                                            Color(N * N, N * N, 0.0));
            }
        }
    }

    SUBCASE("multiply") {
        img2 *= f;
        for (unsigned long j = 0; j < N; ++j) {
            for (unsigned long i = 0; i < N; ++i) {
                CHECK_ITERABLE_APPROX_EQUAL(epsilon, img2[std::pair(i, j)],
                                            Color(f * i, f * j, 0.0));
            }
        }
    }
}

TEST_CASE("write_image_ppm") {
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
    RawImage img{N, M};
    for (unsigned long i = 0; i < N; ++i) {
        for (unsigned long j = 0; j < M; ++j) {
            const auto a = static_cast<ColorScalar>(i);
            const auto b = static_cast<ColorScalar>(j);
            img[{i, j}] = Color(a, b, 0.0);
        }
    }

    {
        std::stringstream ss;
        write_image_ppm(ss, img);
        CHECK(ss.str() == output);
    }
}

TEST_CASE("write_image_pfm") {
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
    RawImage img{N, M};
    for (unsigned long i = 0; i < N; ++i) {
        for (unsigned long j = 0; j < M; ++j) {
            const auto a = static_cast<ColorScalar>(i);
            const auto b = static_cast<ColorScalar>(j);
            img[{i, j}] = Color(a, b, 0.0);
        }
    }

    SUBCASE("test") {
        std::stringstream ss;
        write_image_pfm(ss, img);
        const auto s = ss.str();
        REQUIRE(s.size() == output.size());
        for (std::size_t i = 0; i < output.size(); ++i) {
            CHECK(static_cast<std::uint8_t>(s.at(i)) == output.at(i));
        }
    }
}

}} // namespace cpp_raytracing::test
