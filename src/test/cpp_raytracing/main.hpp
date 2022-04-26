#ifndef TEST_CPP_RAYTRACING_MAIN_HPP
#define TEST_CPP_RAYTRACING_MAIN_HPP

#include <iostream>

int main() {
    using namespace std;

    bool success = cpp_raytracing::run_test_suite();

    if (success) {
        cout << "\033[1;32mpassed\033[0m" << endl;
    } else {
        cout << "\033[1;31mfailed\033[0m" << endl;
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
