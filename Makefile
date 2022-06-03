
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
all: test run_all_previews

### CLEAN ##
.PHONY:clean
clean:
	rm -rf build/*
	rm -rf doc/*

### RUN ALL PREVIEWS ###
.PHONY: run_all_previews
run_all_previews: run_example_basic_preview

### EXAMPLE_BASIC ###

.PHONY: run_example_basic_preview
run_example_basic_preview: example_basic
	@mkdir -p $(OUT)
	@echo "example_basic:"
	@echo "rendering preview image"
	@$(BLD)/example_basic \
		--out $(OUT)/example_basic.preview.ppm \
		--resolution_factor 1 \
		--samples 5 \
		--ray_depth 20 \
		--verbose \

.PHONY: run_example_basic
run_example_basic: run_example_basic_preview
	@mkdir -p $(OUT)
	@echo "example_basic:"
	@echo "rendering final image"
	@$(BLD)/example_basic \
		--out $(OUT)/example_basic.ppm \
		--resolution_factor 8 \
		--samples 50 \
		--ray_depth 50 \
		--verbose \


.PHONY: example_basic
example_basic: $(BLD)/example_basic

$(BLD)/example_basic.d: $(SRC)/example_basic.cpp
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -MM -MQ $@ -o $@ $<

$(BLD)/example_basic: $(SRC)/example_basic.cpp $(BLD)/example_basic.d
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) $(CPP_FLAGS_OPENMP) -o $(BLD)/example_basic $(SRC)/example_basic.cpp

# import dependencies for example_basic binary
include $(BLD)/example_basic.d


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
