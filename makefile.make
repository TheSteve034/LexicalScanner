CC=gcc
CFLAGS=-I
DEPS = scanner.h

%.o: &.cpp $(DEPS)
	$(CC) -cpp -o $@ $< $(CFLAGS)

scanner: LexicalScanner.o scanner.o
	$(CC) -o SamuelReedersScanner LexicalScanner.o scanner.o