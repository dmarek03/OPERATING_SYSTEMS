CC = gcc
FLAGS = -Wall
DIR_NAME = build


build: clean
	mkdir $(DIR_NAME)
	$(CC) $(FLAGS) -c main.c -o  $(DIR_NAME)/main.o
	$(CC) $(DIR_NAME)/main.o -o $(DIR_NAME)/pipe_test



pipe_test: build
	@echo "---- Test 1 ----"
	./$(DIR_NAME)/pipe_test 0.000000001 1
	@echo "---- Test 2 ----"
	./$(DIR_NAME)/pipe_test 0.000000001 10
	@echo "---- Test 3 ----"
	./$(DIR_NAME)/pipe_test 0.000000001 300


clean:
	rm -rf $(DIR_NAME) report.txt