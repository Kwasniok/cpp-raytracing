#!/usr/bin/bash

set -e

EXEC="build/examples/basic_twisted_orb"

make ${EXEC}

mkdir -p out/profiles

valgrind \
    --tool=callgrind \
    --callgrind-out-file="out/profiles/callgrind.out.latest" \
    --dump-instr=yes \
    --collect-jumps=yes \
    ./${EXEC} \
    --out out/out.ppm \
    --samples 1 \
    --ray_depth 50 \

kcachegrind "out/profiles/callgrind.out.latest"
