
INC=include
SRC=src
BLD=build
OUT=out

# GCC
CPP=g++
CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp -I $(INC)
CPP_FLAGS_OPENMP=-fopenmp

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp=libomp -I $(INC)
# CPP_FLAGS_OPENMP=-fopenmp=libomp

.PHONY: all
.DEFAULT_GOAL=all
all: folders main run

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p $(INC) $(SRC) $(BLD) $(OUT)

.PHONY:run
run:
	$(BLD)/main

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


main: $(BLD)/main
$(BLD)/main: $(SRC)/main.cpp $(MAIN_HEADERS)
	$(CPP) $(CPP_FLAGS) $(CPP_FLAGS_OPENMP) -o $(BLD)/main $(SRC)/main.cpp

.PHONY: doc

doc:
	doxygen Doxyfile.in
