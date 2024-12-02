CC = gcc
CFLAGS = -Wall
LIB_NAME = libreverse

all: reverse_test_time_efficiency

.PHONY: all reverse_lib reverse_test build clean reverse_test_time_efficiency reverse_test_correctness

reverse_lib:
	$(CC) $(CFLAGS) -c reverse.c
	ar rcs $(LIB_NAME).a reverse.o


build: clean reverse_lib
	$(CC) $(CFLAGS) main.c $(LIB_NAME).a -o reverse_test


reverse_test_time_efficiency: build
	./reverse_test input.txt output.txt

reverse_test_correctness: build
	./reverse_test input.txt tmp1.txt
	./reverse_test tmp1.txt tmp2.txt
	diff -s input.txt tmp2.txt


clean:
	rm -f *.o *.a build output.txt reverse_test tmp1.txt tmp2.txt


