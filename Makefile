
.PHONY: all
.DEFAULT_GOAL=all
all: folders main

# ensure folders exist
.PHONY: folders
folders:
	mkdir -p src bin out

.PHONY: main
main: bin/main
bin/main: src/main.cpp
	g++ -o bin/main src/main.cpp
