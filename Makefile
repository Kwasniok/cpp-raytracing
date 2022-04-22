
SRC=src
BIN=bin
OUT=out

# GCC
CPP=g++
CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall -std=c++20 -Ofast -g -fopenmp=libomp

.PHONY: all
.DEFAULT_GOAL=all
all: folders main run

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p $(SRC) $(BIN) $(OUT)

.PHONY:run
run:
	$(BIN)/main

.PHONY: main

MAIN_HEADERS= \
	$(SRC)/camera.hpp \
	$(SRC)/color.hpp \
	$(SRC)/hittable.hpp \
	$(SRC)/image.hpp \
	$(SRC)/material.hpp  \
	$(SRC)/random.hpp  \
	$(SRC)/ray.hpp \
	$(SRC)/renderer.hpp  \
	$(SRC)/scalar.hpp  \
	$(SRC)/scene.hpp  \
	$(SRC)/sphere.hpp \
	$(SRC)/util.hpp  \
	$(SRC)/vec3.hpp \


main: $(BIN)/main
$(BIN)/main: $(SRC)/main.cpp $(MAIN_HEADERS)
	$(CPP) $(CPP_FLAGS) -o $(BIN)/main $(SRC)/main.cpp

.PHONY: doc

doc:
	doxygen Doxyfile.in
