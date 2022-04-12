
SRC=src
BIN=bin
OUT=out

CPP=g++
CPP_FLAGS=-std=c++20 -O3

.PHONY: all
.DEFAULT_GOAL=all
all: folders main run

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p $(SRC) $(BIN) $(OUT)

.PHONY:run
run:
	$(BIN)/main > $(OUT)/out.ppm

.PHONY: main
main: $(BIN)/main
$(BIN)/main: $(SRC)/main.cpp
	$(CPP) $(CPP_FLAGS) -o $(BIN)/main $(SRC)/main.cpp
