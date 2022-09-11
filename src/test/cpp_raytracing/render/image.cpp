#include <array>
#include <sstream>

#include <cpp_raytracing/render/image.hpp>

#include <cpp_raytracing_test.hpp>

namespace cpp_raytracing { namespace test {

void test_constructor() {
    RawImage(10, 10);
}

void test_properties() {
    const RawImage img{10, 20};
    TEST_ASSERT_EQUAL(img.width(), 10ul);
    TEST_ASSERT_EQUAL(img.height(), 20ul);
}

void test_operator_bracket() {
    constexpr Color color{0.0, 0.5, 1.0};
    RawImage img{10, 10};
    // non-const
    {
        img[{1, 2}] = color;
        TEST_ASSERT_EQUAL(img[std::pair(1, 2)], color);
    }
    // const
    {
        const RawImage& cimg = img;
        TEST_ASSERT_EQUAL(cimg[std::pair(1, 2)], color);
    }
}

void test_arithmetic() {
    constexpr unsigned long N = 8;
    constexpr ColorScalar f = 8.0;
    RawImage img1{N, N};
    RawImage img2{N, N};
    for (unsigned long j = 0; j < N; ++j) {
        for (unsigned long i = 0; i < N; ++i) {
            img1[{i, j}] = Color(N * N - i, N * N - j, 0.0);
            img2[{i, j}] = Color(i, j, 0.0);
        }
    }
    // add
    {
        img1 += img2;
        for (unsigned long j = 0; j < N; ++j) {
            for (unsigned long i = 0; i < N; ++i) {
                TEST_ASSERT_EQUAL(img1[std::pair(i, j)],
                                  Color(N * N, N * N, 0.0));
            }
        }
    }
    // multiply
    {
        img2 *= f;
        for (unsigned long j = 0; j < N; ++j) {
            for (unsigned long i = 0; i < N; ++i) {
                TEST_ASSERT_EQUAL(img2[std::pair(i, j)],
                                  Color(f * i, f * j, 0.0));
            }
        }
    }
}

void test_write_image_ppm() {
    constexpr unsigned long N = 2;
    constexpr unsigned long M = 3;
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
            img[{i, j}] = Color(i, j, 0.0);
        }
    }
    {
        std::stringstream ss;
        write_image_ppm(ss, img);
        TEST_ASSERT_EQUAL(ss.str(), output);
    }
}

void test_write_image_pfm() {
    constexpr unsigned long N = 2;
    constexpr unsigned long M = 3;
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
            img[{i, j}] = Color(i, j, 0.0);
        }
    }
    {
        std::stringstream ss;
        write_image_pfm(ss, img);
        const auto s = ss.str();
        TEST_ASSERT_EQUAL(s.size(), output.size());
        for (std::size_t i = 0; i < output.size(); ++i) {
            TEST_ASSERT_EQUAL(static_cast<std::uint8_t>(s[i]), output[i]);
        }
    }
}

void run_test_suite() {
    run(test_constructor);
    run(test_properties);
    run(test_operator_bracket);
    run(test_arithmetic);
    run(test_write_image_ppm);
    run(test_write_image_pfm);
}

}} // namespace cpp_raytracing::test
