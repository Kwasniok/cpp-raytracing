#include <iostream>

#include "color.hpp"

using namespace std;
using namespace ray;

void print_example_ppm_file() {
    // see: https://en.wikipedia.org/wiki/Netpbm#File_formats

    const unsigned long width = 100;
    const unsigned long height = 100;
    const unsigned long max_color = 255;

    // header
    cout << "P3 # ASCII RGB" << endl;
    cout << width << " " << height << " # width x height" << endl;
    cout << max_color << " # max color value per channel" << endl;

    // content (checker board)
    cerr << "Rendering image ..." << endl;
    for (long j = 0; j < height; ++j) {
        cerr << "line " << (j + 1) << "/" << height << endl;
        for (long i = 0; i < width; ++i) {
            Color color{double(i) / (width - 1), double(j) / (height - 1),
                        0.25};
            cout << "    ";
            write_color_as_integrals(cout, color);
        }
        cout << endl;
    }
    cerr << "Done." << endl;
}

int main(int argc, char** argv) { print_example_ppm_file(); }
