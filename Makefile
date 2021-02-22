CC?=clang
CXX?=clang++
CFLAGS=-std=c++98 -O3
LFLAGS=

clean:
	rm ./svgasm

svgasm:
	$(CXX) $(CFLAGS) $(LFLAGS) -o ./svgasm ./src/svgasm.cpp

all: svgasm

.PHONY: all clean
