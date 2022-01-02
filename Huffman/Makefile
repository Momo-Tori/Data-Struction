CC=g++
CFLAGS=-m64 -static -O3
DEPS = Huffman.h
OBJ = Huffman.o main.o 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

Huffman: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

all: Huffman

.PHONY: clean

clean:
	rm -rf Huffman
	rm *.o