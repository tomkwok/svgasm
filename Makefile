CC?=clang
CXX?=clang++
CFLAGS=-std=c++98 -O3
LFLAGS=

svgasm:
	$(CXX) $(CFLAGS) -o ./svgasm ./src/svgasm.cpp $(LFLAGS)

clean:
	rm ./svgasm

all: svgasm

.PHONY: all clean
