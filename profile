#!/usr/bin/bash

make build/profile

mkdir -p out/profiles

./build/profile \
    --out out/out.ppm \
    --samples 50 \
    --resolution_factor 2 \
    --ray_depth 25 \

mv gmon.out out/profiles/

gprof ./build/profile out/profiles/gmon.out | less
