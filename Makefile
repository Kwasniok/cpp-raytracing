
INC=include
SRC=src
BIN=bin
OUT=out

# GCC
CPP=g++
CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp -I $(INC)

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp=libomp -I $(INC)

.PHONY: all
.DEFAULT_GOAL=all
all: folders main run

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p $(INC) $(SRC) $(BIN) $(OUT)

.PHONY:run
run:
	$(BIN)/main

.PHONY: main

MAIN_HEADERS= \
	$(INC)/cpp_raytracing/camera.hpp \
	$(INC)/cpp_raytracing/color.hpp \
	$(INC)/cpp_raytracing/hittable.hpp \
	$(INC)/cpp_raytracing/image.hpp \
	$(INC)/cpp_raytracing/material.hpp  \
	$(INC)/cpp_raytracing/random.hpp  \
	$(INC)/cpp_raytracing/ray.hpp \
	$(INC)/cpp_raytracing/renderer.hpp  \
	$(INC)/cpp_raytracing/scalar.hpp  \
	$(INC)/cpp_raytracing/scene.hpp  \
	$(INC)/cpp_raytracing/sphere.hpp \
	$(INC)/cpp_raytracing/util.hpp  \
	$(INC)/cpp_raytracing/vec3.hpp \


main: $(BIN)/main
$(BIN)/main: $(SRC)/main.cpp $(MAIN_HEADERS)
	$(CPP) $(CPP_FLAGS) -o $(BIN)/main $(SRC)/main.cpp

.PHONY: doc

doc:
	doxygen Doxyfile.in
