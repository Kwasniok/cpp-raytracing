
INC=include
SRC=src
BLD=build
OUT=out

INCLUDES=-I third_party/glm -I third_party/argparse/include

# GCC
CPP=g++
CPP_FLAGS=-Wall -pedantic -std=c++20 -Ofast -g -fopenmp -I $(INC) $(INCLUDES)

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall -pedantic -std=c++20 -Ofast -g -fopenmp=libomp -I $(INC) $(INCLUDES)


### ALL (DEFAULT) ###
.PHONY: all
.DEFAULT_GOAL=all
all: test run_all

### CLEAN ##
.PHONY:clean
clean:
	rm -rf build/*
	rm -rf doc/*

### RUN ###
.PHONY: run_all
run_all: run_example_01

### EXAMPLE 01 ###

.PHONY: run_example_01
run_example_01: example_01
	@mkdir -p $(OUT)
	@echo "rendering preview image"
	@$(BLD)/example_01 \
		--out $(OUT)/example_01.preview.ppm \
		--resolution_factor 1 \
		--samples 5 \
		--ray_depth 20 \
		--verbose \

	@echo "rendering final image"
	@$(BLD)/example_01 \
		--out $(OUT)/example_01.ppm \
		--resolution_factor 8 \
		--samples 50 \
		--ray_depth 50 \
		--verbose \


.PHONY: example_01
example_01: $(BLD)/example_01
$(BLD)/example_01.d: $(SRC)/example_01.cpp
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -MM -MQ $@ -o $@ $<

$(BLD)/example_01: $(SRC)/example_01.cpp $(BLD)/example_01.d
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) $(CPP_FLAGS_OPENMP) -o $(BLD)/example_01 $(SRC)/example_01.cpp

# import dependencies for main binary
include $(BLD)/example_01.d


### DOCUMENTATION ###
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
	@# loop is exited after first failure
	@set -e; for T in $(TESTS); do printf "test: $$T "; $$T; done

$(TEST_DEPENDS): $(BLD)/%.d: $(SRC)/%.cpp
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -MM -MQ $@ -o $@ $<

$(TESTS): $(BLD)/%: $(SRC)/%.cpp $(BLD)/%.d
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -o $@ $<

# import dependencies for all target binaries
include $(TEST_DEPENDS)
