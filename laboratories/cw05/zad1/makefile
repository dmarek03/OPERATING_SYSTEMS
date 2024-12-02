CC = gcc
FLAGS = -Wall
DIR_NAME = build
.PHONY: all clean signal_test build

all:
	signal_test

build: clean
	mkdir $(DIR_NAME)
	$(CC) $(FLAGS) -c main.c -o  $(DIR_NAME)/main.o
	$(CC) $(DIR_NAME)/main.o -o $(DIR_NAME)/signal_test


signal_test: build

	@echo "---- Test 1----"
	./$(DIR_NAME)/signal_test "ignore"
	@echo "---- Test 2 ----"
	./$(DIR_NAME)/signal_test "handler"
	@echo "---- Test 3 ----"
	./$(DIR_NAME)/signal_test "mask"
	@echo "---- Test 4  ----"
	./$(DIR_NAME)/signal_test "none"
	@echo "---- Test 5----"
	./$(DIR_NAME)/signal_test "invalid option"





clean:
	rm -rf $(DIR_NAME)