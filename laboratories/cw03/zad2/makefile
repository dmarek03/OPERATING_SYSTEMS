CC = gcc
FLAGS = -Wall

.PHONY: clean test

all: test

browse: clean
	$(CC) $(FLAGS)  main.c -o browse

test: browse
	./browse ./
	wc --bytes *



clean:
	rm -f test