
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
all: test main run

.PHONY:run
run:
	@mkdir -p $(OUT)
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
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) $(CPP_FLAGS_OPENMP) -o $(BLD)/main $(SRC)/main.cpp

.PHONY: doc

doc:
	@mkdir -p doc
	doxygen Doxyfile.in


### TESTS ###

TEST_SOURCES = $(shell find $(SRC)/test -name '*.cpp')
TEST_DEPENDS = $(TEST_SOURCES:$(SRC)/%.cpp=$(BLD)/%.d)
TEST_BINARIES = $(TEST_SOURCES:$(SRC)/%.cpp=$(BLD)/%)
TESTS = $(filter $(BLD)/test/%, $(TEST_BINARIES))

.PHONY: test
test: $(TESTS);
	@set -e; for T in $(TESTS); do printf "test: $$T ... "; $$T; done

$(TEST_DEPENDS): $(BLD)/%.d: $(SRC)/%.cpp
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -MM -MQ $@ -o $@ $<

$(TESTS): $(BLD)/%: $(SRC)/%.cpp $(BLD)/%.d
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -o $@ $<

# import dependencies for all target binaries
include $(TEST_DEPENDS)
