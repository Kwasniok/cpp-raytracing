# Ray Tracing

This project is based on https://raytracing.github.io/ and includes some additional twists.

## Features

- Materials
![materials: diffuse glass metal emitter](https://user-images.githubusercontent.com/7516208/172666620-63556ff1-4056-4c56-963e-976d66ede688.png)

- Rolling Shutter & Motion Blur
![rolling shutter: rotor at rest](https://user-images.githubusercontent.com/7516208/172665290-341e4786-5dcd-45e0-b850-2956b121007c.png)
![rolling shutter: rotor in motion](https://user-images.githubusercontent.com/7516208/172665270-88ba7a69-5337-4cf6-bba5-7247ee334ffb.png)

- Volumes
    - Constant Density & Convex Boundary
     ![constant density mist](https://user-images.githubusercontent.com/7516208/173927126-295bf76b-6b50-4f72-a997-fbb6b0a3b902.png)

- Multi-Threaded
    - Rendering
    - BVH Tree Generation (Partially)

- Bounding Volume Hierachy (BVH)


## Build, Test & Preview
```bash
make
```
See `./out` directory for the output.

## Run
```bash
./build/<executable>
```
Where `<executable>` is any of the provided executables.
Use `--help` to see the respective usage.

## Build Documentation
```bash
make doc
```
See `doc/html/index.html` for the documentation.

## Auto-Format
Run
```bash
.auto_format
```
to automatically format the entire code base (requires `clang-format`).

## Debug
Add `DEBUG=1` to `make`. The executables will be placed in `build/debug` instead.

## Profile
Run
```bash
./profile
```
to get a profile (requires `valgrind` (profiler) and `kcachegrind` (show data)).

## Requirements

- Linux kernel v5.17+
- C++20
- gcc v12.1+
- make v4.3+
- doxygen v1.9+ (optional)
- valgrind v3.18+ (optional)
- kcachegrind v22.04+ (optional)
