# Ray Tracing

- based on: https://raytracing.github.io/

![diffuse_metal_glass_emitter_defocus_highres](https://user-images.githubusercontent.com/7516208/164042906-fce16f88-9789-44f0-9081-7691013a5fb1.png)

## Build
```bash
make main
```

## Run
```bash
./build/main
```
For the image see `out` directory.

## Build Documentation
```bash
make doc
```
See `doc/html/index.html` for the documentation.

## Auto-Format
Run
```
.auto_format
```
to automatically format the entire code base (requires `clang-format`).

## Requirements

- Linux kernel v5.17+
- C++20
- gcc v12.1+
- make v4.3+
- doxygen v1.9+
- valgrind v3.18+ (optional)

## Features
### (Translational) Motion Blur
![out](https://user-images.githubusercontent.com/7516208/171229392-9d9960a9-1b39-418b-8a5e-fca56b6c3676.png)

