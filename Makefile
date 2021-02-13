CC=clang++
CFLAGS=-std=c++11 -O3
LFLAGS=

clean:
	rm ./svgasm

svgasm:
	$(CC) $(CFLAGS) $(LFLAGS) -o ./svgasm ./svgasm.cpp

all: svgasm

.PHONY: all clean
