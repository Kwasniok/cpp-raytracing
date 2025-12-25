#!/usr/bin/env bash

function timestamp {
    echo "        $(date +%H:%M:%S)"
}

function out {
    FRAME=$1
    echo "${OUT_DIR}/${FRAME}"
}

function ppm {
    FRAME=$1
    echo "${OUT_DIR}/${FRAME}.ppm"
}

function render_frame {
    FRAME=$1
    TIME=$(echo "${FRAME}/${FPS}" | bc -l)
    DYNAMIC_ARGS=$(dynamic_args ${TIME})
    echo "frame = ${FRAME}, time = ${TIME}"
    ${EXEC} \
    --out $(out ${FRAME}) \
    ${FIXED_ARGS} \
    ${DYNAMIC_ARGS} \
    > /dev/null

}

function make_frames {
    timestamp
    for FRAME in $(seq -f "%05g" ${FRAME_START} ${FRAME_STOP})
    do
        render_frame ${FRAME}
        timestamp
    done

}

function convert_to_mp4 {
    ffmpeg -r ${FPS} -i $(ppm "%05d") -vcodec libx264 -pix_fmt yuv420p -y "${OUT_DIR}/mov.mp4"
}


# entry point

function make_movie {
    make "${EXEC}"
    mkdir -p "${OUT_DIR}"

    echo "make movie for ${EXEC}:"
    make_frames
    convert_to_mp4
}