CC = gcc
FLAGS = -Wall
DIR_NAME = build

.Phony: clean build test

all:
	test

build: clean
	mkdir $(DIR_NAME)
	$(CC) $(FLAGS)  main.c -o $(DIR_NAME)/process_test


test: build
	mkdir test_dir
	touch test_dir/file_1
	touch test_dir/file_2
	@echo "---- Test 1 ----"
	./$(DIR_NAME)/process_test test_dir
	@echo "---- Test 2 ----"
	./$(DIR_NAME)/process_test nonexistingdir
	@echo "---- Test 3 ----"
	./$(DIR_NAME)/process_test /etc


clean:
	rm -rf $(DIR_NAME) test_dir


