
.PHONY: all
.DEFAULT_GOAL=all
all: folders main

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p src bin

.PHONY: main
main: bin/main
bin/main: src/main.cpp
	gcc -o bin/main src/main.cpp
