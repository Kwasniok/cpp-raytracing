#!/usr/bin/bash

make build/examples/benchmark

mkdir -p out/profiles

valgrind \
    --tool=callgrind \
    --callgrind-out-file="out/profiles/callgrind.out.latest" \
    ./build/examples/benchmark \
    --out out/out.ppm \
    --samples 50 \
    --ray_depth 50 \

kcachegrind "out/profiles/callgrind.out.latest"
