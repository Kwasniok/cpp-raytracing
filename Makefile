
INC:=include
SRC:=src
BLD:=build
OUT:=out

INCLUDES=-I third_party/glm -I third_party/argparse/include

# GCC
CPP=g++
CPP_FLAGS=-Wall -pedantic -std=c++20 -g -fopenmp -I $(INC) $(INCLUDES)

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall -pedantic -std=c++20 -g -fopenmp=libomp -I $(INC) $(INCLUDES)


DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CPP_FLAGS += -O0 -DDEBUG
    BLD:=$(BLD)/debug
else
    CPP_FLAGS += -Ofast
endif

### ALL (DEFAULT) ###
.PHONY: all
.DEFAULT_GOAL=all
all: test example_previews

### CLEAN ##
.PHONY:clean
clean:
	rm -rf build/*
	rm -rf doc/*

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


### EXAMPLES ###

EXAMPLE_SOURCES = $(shell find $(SRC)/examples -name '*.cpp')
EXAMPLE_DEPENDS = $(EXAMPLE_SOURCES:$(SRC)/%.cpp=$(BLD)/%.d)
EXAMPLE_BINARIES = $(EXAMPLE_SOURCES:$(SRC)/%.cpp=$(BLD)/%)
EXAMPLES = $(filter $(BLD)/examples/%, $(EXAMPLE_BINARIES))

$(EXAMPLE_DEPENDS): $(BLD)/%.d: $(SRC)/%.cpp
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -MM -MQ $@ -o $@ $<

$(EXAMPLES): $(BLD)/%: $(SRC)/%.cpp $(BLD)/%.d
	@mkdir -p $(@D) # provide parent directory of target
	$(CPP) $(CPP_FLAGS) -o $@ $<

# import dependencies for all example binaries
include $(EXAMPLE_DEPENDS)

.PHONY: example_previews
example_previews: $(EXAMPLES);
	@mkdir -p $(OUT)
	@for E in $(EXAMPLES); \
	do \
		echo "rendering preview for $$E"; \
		$$E \
		--out $(OUT)/example_$${E##*/}.preview.ppm \
		--resolution_factor 1 \
		--samples 5 \
		--ray_depth 20 \
		#--verbose \
	done
