# Ray Tracing

- based on: https://raytracing.github.io/

![diffuse_metal_glass_emitter_defocus_highres](https://user-images.githubusercontent.com/7516208/164042906-fce16f88-9789-44f0-9081-7691013a5fb1.png)

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
Add `DEBUG=1` to `make`. The executables will be placed in `build/debug` command.

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
