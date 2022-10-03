# Ray Tracing

This project is based on https://raytracing.github.io/ and includes some
additional twists like curved space.

Note: For a conventional linear ray tracer see commit b6bbcfe.

## Features

### Adaptive Non-Linear Ray Propagation

![basic_swirl highres](https://user-images.githubusercontent.com/7516208/193019366-d81259c3-5ef9-402e-a46c-b20b547051ad.png)

Created with `build/examples/basic_swirl` (version ee5499f) using `swirl_strength = 0.01` and `resolution_factor = 4`.

![basic_twisted_orb highres](https://user-images.githubusercontent.com/7516208/193018890-9b75b3b1-5479-45ee-9d46-a07620b4182a.png)

Created with `build/examples/basic_twisted_orb` (version f952428) using `twist_angle = -6.0`, `resolution_factor = 4`, `ray_error_abs  = 1e-12` and `ray_depth = 10000`.

### Materials

![materials: diffuse glass metal emitter](https://user-images.githubusercontent.com/7516208/172666620-63556ff1-4056-4c56-963e-976d66ede688.png)

Diffuse, Dielectric, Metal & Emission

### Rolling Shutter & Motion Blur

![rolling shutter: rotor in motion](https://user-images.githubusercontent.com/7516208/172665270-88ba7a69-5337-4cf6-bba5-7247ee334ffb.png)

Wheel in Motion.

![rolling shutter: rotor at rest](https://user-images.githubusercontent.com/7516208/172665290-341e4786-5dcd-45e0-b850-2956b121007c.png)

Resting.

### Optimizations
- Multi-Threaded
    - Rendering (fully)
    - BVH Tree Generation (partially)
- Bounding Volume Hierachy (BVH)


## Performance

| | ![basic_twisted_orb twist_angle -6 err_abs e-10](https://user-images.githubusercontent.com/7516208/192962185-89160c38-3620-4540-af3a-fbbe8e942959.png) | ![basic_twisted_orb twist_angle -3 err_abs e-10](https://user-images.githubusercontent.com/7516208/192967775-2a06da8e-ee4b-41ab-a1a6-5d22074014d6.png) | ![basic_twisted_orb angle_0 err_abs_e-10](https://user-images.githubusercontent.com/7516208/192968225-010ed177-3f5e-46f5-8a9f-00d100836863.png) |
|-|-|-|-|
| geometry | curved | curved | flat |
| method | adaptive Cash-Karp | adaptive Cash-Karp | adaptive Cash-Karp |
file (commit f229e20) | `examples/basic_twisted_orb` | `examples/basic_twisted_orb` | `examples/basic_twisted_orb` |
| size (`resolution_factor`) | 480x270 pixel (`2`) | 480x270 pixel (`2`) | 480x270 pixel (`2`) |
| `samples` | `100` | `100` | `100` |
| `twist_angle` | `-6.0` | `-3.0` | `0.0` |
| `ray_error_abs` | `1e-10` | `1e-10` | `1e-3` |
| user time (intel i5-4590) | **61min31sec** | **47min17sec** | **2min41sec** |

| | ![basic_swirl swirl_strength 0.01 err_abs_e-10](https://user-images.githubusercontent.com/7516208/192981000-2349c392-176a-4f33-be48-0b0274f01479.png) | ![basic_swirl 0 err_abs_e-10](https://user-images.githubusercontent.com/7516208/192981968-11b7b375-a414-477b-a0dc-6a2792e8b95b.png) | ![basic_euclidean](https://user-images.githubusercontent.com/7516208/192982282-e9f082dc-b6a0-4955-b049-eed96a2e908e.png) |
|-|-|-|-|
| geometry | curved | flat | flat |
| method | adaptive Cash-Karp | adaptive Cash-Karp | linear |
file (commit 2cc272e) | `examples/basic_swirl` | `examples/basic_swirl` | `examples/basic_euclidean` |
| size (`resolution_factor`) | 480x270 pixel (`2`) | 480x270 pixel (`2`) | 480x270 pixel (`2`) |
| `samples` | `100` | `100` | `100` |
| `swirl_strength` | `0.01` | `0.0` | - |
| `ray_error_abs` | `1e-10` | `1e-10` | - |
| user time (intel i5-4590) | **30min11sec** | **5min47sec** | **9sec** |

note: The image in the middle and right are identical due to the strong similarity of the renderers and usage of pseudo-random number generators with fixed seeds.

## Build, Test & Preview
```bash
make
```
See `./build` for the executables and  `./out` directory for the test image output.

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
- C++ Boost v1.80+
- gcc v12.2+ or glang v14.0+
- make v4.3+
- doxygen v1.9+ (optional, documentation)
- netpbm v10.73+ (optional, image processing)
- imagemagick v7.1+ (optional, image processing)
- darktable v3.8+ (optional, image processing)
- clang-format v14.0+ (optional, development)
- clang-tidy v14.0+ (optional, development)
- valgrind v3.18+ (optional, debug & profiling)
- kcachegrind v22.04+ (optional. debug & profiling)

## General Remarks

- The heavy usage of headers is intended for future use of c++ modules.
  Unfortunately, c++ modules are currently (Sep 2022) not widespread enough to
  be considered supported by all compilers.

## Movies

### Curved
https://user-images.githubusercontent.com/7516208/191520786-c169b980-c4e6-4899-bd09-c5c95d53e7d6.mp4

### Flat
https://user-images.githubusercontent.com/7516208/191520742-2c9fdb1e-5f0b-4696-b266-64c635c4e64b.mp4

| geometry | curved | flat |
|-|-|-|
| method | non-linear propagation | linear/conventional propagation |
file (commit ed4358e) | `examples/swirl_triangles` | `examples/triangles` |
| size (`resolution_factor`) | 480x270 pixel (`2`) | 480x270 pixel (`2`) |
| frames | `60` | `60` |
| `samples` | `1000` | `1000` |
| `ray_depth` | `500` | `500` |
| `ray_step_size` | `0.1` | - |
| `swirl_strength` | `0.01` | - |
| wall time (AMD Ryzen 5 5600X) | **2.5days** | **7min** |
| max. total ray segments | 3,888,000,000,000 | 3,888,000,000,000 |

[netpbm]: https://en.wikipedia.org/wiki/Netpbm
[darktable]: https://www.darktable.org/
