
.PHONY: all
all: ../bin/test ../bin/test2

../bin/test: test.cpp
	g++ -I ../PEGTL/include --std=c++20 test.cpp -o ../bin/test -O0 -ggdb -Wall 

../bin/test2: test2.cpp
	g++ -I ../PEGTL/include --std=c++20 test2.cpp -o ../bin/test2 -O0 -ggdb -Wall
