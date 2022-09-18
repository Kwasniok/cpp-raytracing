# Ray Tracing

This project is based on https://raytracing.github.io/ and includes some
additional twists like curved space.

## Features

### Non-Linear Ray Propagation


| | ![non-linear: swirl geometry 2400 samples](https://user-images.githubusercontent.com/7516208/190710403-8e48caee-bbc7-451e-9fdd-7639b1a749be.png) | ![non-linear: reference swirl geometry 2400 samples](https://user-images.githubusercontent.com/7516208/190714403-650472f9-7f9a-4eaf-8182-0ca079bbcb3d.png) | ![linear: reference flat geometry 2400 samples](https://user-images.githubusercontent.com/7516208/190715517-52e6dd32-8ede-45fb-98d6-1c9abb5c8fbc.png) |
|-|-|-|-|
| geometry | curved | flat | flat |
| method | non-linear propagation | non-linear propagation | linear/conventional propagation |
file (commit f706fcf) | `examples/swirl_triangles` | `examples/swirl_triangles` | `examples/triangles` |
| size (`resolution_factor`) | 240x135 pixel (`1`) | 240x135 pixel (`1`) | 240x135 pixel (`1`) |
| `samples` | `2400` | `2400` | `2400` |
| `ray_depth` | `1000` | `50` | `50` |
| `ray_step_size` | `0.1` | `1000` | - |
| `swirl_strength` | `0.01` | `0.0` | - |
| user time (intel i5-4590) | **833min47sec** | **41min31sec** | **1min49sec** |
| max. total ray segments | 77,760,000,000 | 3,888,000,000 | 3,888,000,000 |

note: The image in the middle and right are identical due to the strong similarity of the renderers and usage of pseudo-random number generators with fixed seeds.

### Materials
- Diffuse, Dielectric, Metal & Emission
    ![materials: diffuse glass metal emitter](https://user-images.githubusercontent.com/7516208/172666620-63556ff1-4056-4c56-963e-976d66ede688.png)

### Rolling Shutter & Motion Blur
![rolling shutter: rotor at rest](https://user-images.githubusercontent.com/7516208/172665290-341e4786-5dcd-45e0-b850-2956b121007c.png)
![rolling shutter: rotor in motion](https://user-images.githubusercontent.com/7516208/172665270-88ba7a69-5337-4cf6-bba5-7247ee334ffb.png)

### Optimizations
- Multi-Threaded
    - Rendering (fully)
    - BVH Tree Generation (partially)
- Bounding Volume Hierachy (BVH)


## Build, Test & Preview
```bash
make
```
See `./out` directory for the output.

## Documentation
```bash
make doc
```
See `./doc` directory for the documentation.


## Run
```bash
./build/examples/<executable>
```
Where `<executable>` is any of the provided executables.
Use `--help` to see the respective usage.

## Image File Formats & Processing
The current image formats focus on ease on implementation and depend on [netpbm][netpbm].
- PPM (Protable PixelMap also known as PNM) dircrete netpbm format (similar to PNG, JPEG etc.)
- PFM (Protable FloatMap) floating point netpbm format (similar to HDR, RAW etc.)

To convert to the desired formats via the netpbm package.
E.g.
```bash
pnmtopng $file.pnm > $file.png
pfmtopam $file.pfm > $file.pam
pamtopng $file.pam > $file.PNG
# ...
```

Or use imagemagick. E.g.
```bash
magick $file.pfm -gamma 1.0 -set gamma 1.0 $file.png
magick $file.pfm -gamma 1.0 -set gamma 1.0 $file.tiff
```

Proper exposure and color grading can be archived the same way as for images
taken by real cameras. E.g. with [darktable][darktable].

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
- doxygen v1.9+ (optional, documentation)
- netpbm v10.73+ (optional, image processing)
- imagemagick v7.1+ (optional, image processing)
- darktable v3.8+ (optional, image processing)
- valgrind v3.18+ (optional, debug & profiling)
- kcachegrind v22.04+ (optional. debug & profiling)

## General Remarks

- The heavy usage of headers is intended for future use of c++ modules.
  Unfortunately, c++ modules are currently (Sep 2022) not widespread enough to
  be considered supported by all compilers.

[netpbm]: https://en.wikipedia.org/wiki/Netpbm
[darktable]: https://www.darktable.org/
