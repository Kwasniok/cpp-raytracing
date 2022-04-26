
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


.PHONY: io

IO_HEADERS= \
	$(SRC)/io/all.hpp \
	$(SRC)/io/base.hpp \
	$(SRC)/io/scalar.hpp \

io: $(BIN)/io.o
$(BIN)/io.o: $(IO_HEADERS)
	$(CPP) $(CPP_FLAGS) -o $(BIN)/io.o -c $(SRC)/io.cpp  -I PEGTL/include


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
$(BIN)/main: $(BIN)/io.o $(SRC)/main.cpp $(MAIN_HEADERS)
	$(CPP) $(CPP_FLAGS) -o $(BIN)/main $(SRC)/main.cpp $(BIN)/io.o

.PHONY: doc

doc:
	doxygen Doxyfile.in
