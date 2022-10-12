#include <cpp_raytracing.hpp>
#include <range/v3/view.hpp>

using namespace cpp_raytracing;

Mat3x3 test_function(const Mat3x3& A, const Mat3x3& B) {
    return A * B;
}

int f() {
    const Mat3x3 A = {
        Vec3{0, 3, 6},
        Vec3{1, 4, 7},
        Vec3{2, 5, 8},
    };
    const Mat3x3 B = {
        Vec3{4, 7, 10},
        Vec3{5, 8, 11},
        Vec3{6, 9, 12},
    };
    return test_function(A, B)[0][0];
}

int main() {
    int res{};
    for (const auto i : ranges::view::iota(0, INT_MAX)) {
        res += f() * i;
    }
    return res;
}
