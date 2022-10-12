#include <range/v3/view.hpp>
#include <ttl/ttl.h>

using namespace ttl;
using Mat3x3 = Tensor<2, 3, double>;

constexpr const Index<'i'> i{};
constexpr const Index<'j'> j{};
constexpr const Index<'k'> k{};

Mat3x3 test_function(const Mat3x3& A, const Mat3x3& B) {
    Mat3x3 C = {};
    C(i, k) = A(j, i) * B(k, j);
    return C;
}

int f() {

    const Mat3x3 A = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    const Mat3x3 B = {4, 5, 6, 7, 8, 9, 10, 11, 12};

    return test_function(A, B)[0][0];
}

int main() {
    int res{};
    for (const auto i : ranges::view::iota(0, INT_MAX)) {
        res += f() * i;
    }
    return res;
}
