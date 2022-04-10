#include <iostream>

using namespace std;

void print_example_ppm_file() {
    // see: https://en.wikipedia.org/wiki/Netpbm#File_formats

    constexpr unsigned long width = 100;
    constexpr unsigned long height = 100;

    // header
    cout << "P1 # bitmap" << endl;
    cout << width << " " << height << " # width x height" << endl;

    // content
    for (unsigned long i = 0; i < width; ++i) {
        for (unsigned long j = 0; j < height; ++j) {
            if ((i + j) % 2 == 0) {
                cout << " 0";
            } else {
                cout << " 1";
            }
        }
        cout << endl;
    }
}

int main(int argc, char** argv) { print_example_ppm_file(); }
