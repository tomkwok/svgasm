CC=clang
CXX=clang++
CFLAGS=-std=c++98 -O3
LFLAGS=

clean:
	rm ./svgasm

svgasm:
	$(CXX) $(CFLAGS) $(LFLAGS) -o ./svgasm ./svgasm.cpp

all: svgasm

.PHONY: all clean
