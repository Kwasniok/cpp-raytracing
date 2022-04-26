#ifndef CPP_RAYTRACING_IO_SCALAR_H
#define CPP_RAYTRACING_IO_SCALAR_H

#include "base.hpp"

#include "../scalar.hpp"

namespace ray::io {

std::ostream& write(std::ostream& os, const Scalar& scalar) {
    os << std::fixed << std::setprecision(10);
    os << scalar;
    return os;
}
std::istream& read(std::istream& is, Scalar& scalar) {
    is >> scalar;
    return is;
}

} // namespace ray::io

#endif
