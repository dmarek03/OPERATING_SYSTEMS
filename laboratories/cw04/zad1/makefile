CC = gcc
FLAGS = -Wall
DIR_NAME = build
.PHONY: clean process_test build


build: clean
	mkdir $(DIR_NAME)
	$(CC) $(FLAGS) -c main.c -o $(DIR_NAME)/main.o
	$(CC) $(DIR_NAME)/main.o -o $(DIR_NAME)/process_test

process_test: build
	$(DIR_NAME)/process_test 3

clean:
	rm -rf $(DIR_NAME)