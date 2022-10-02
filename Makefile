
INC:=include
SRC:=src
BLD:=build
OUT:=out

INCLUDES= \
	-I $(INC) \
	-I third_party/glm \
	-I third_party/argparse/include \

# GCC
CPP=g++
CPP_FLAGS=-Wall \
	-Wextra \
	-pedantic \
	-Wnon-virtual-dtor \
	-Wold-style-cast \
	-Wunused \
	-Woverloaded-virtual \
	-Wimplicit-fallthrough \
	-std=c++20 \
	-g \
	-fopenmp \
	$(INCLUDES)\

CPP_PROFILE_FLAGS=$(CPP_FLAGS) -pg

# CLANG
# CPP=clang++
# CPP_FLAGS=-Wall \
# 	-Wextra \
# 	-pedantic \
# 	-Wnon-virtual-dtor \
# 	-Wold-style-cast \
# 	-Wunused \
# 	-Woverloaded-virtual \
# 	-Wimplicit-fallthrough \
# 	-Wdocumentation \
# 	-std=c++20 \
# 	-g \
# 	-fopenmp=libomp \
# 	$(INCLUDES)\

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CPP_FLAGS += -O0 -DDEBUG
    BLD:=$(BLD)/debug
else
    CPP_FLAGS += -Ofast
endif

### DEFAULT ###
.PHONY: default
.DEFAULT_GOAL=default
default: test run_examples

### ALL ###
.PHONY: all
all: test doc run_examples

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


.PHONY: examples
examples: $(EXAMPLES);

.PHONY: run_examples
run_examples: $(EXAMPLES);
	@mkdir -p $(OUT)
	@for E in $(EXAMPLES); \
	do \
		echo "rendering preview for $$E"; \
		$$E \
		--out $(OUT)/example_$${E##*/}.preview \
		#--verbose \
	done
