CC = gcc
FLAGS = -Wall
DIR_NAME = build

.PHONY: all clean santa_and_reindeer_simulation

all: santa_and_reindeer_simulation

santa_and_reindeer_simulation: main.c
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) main.c -o $(DIR_NAME)/santa_and_reindeer_simulation -lpthread


run_santa_and_reindeer_simulation:santa_and_reindeer_simulation
	./$(DIR_NAME)/santa_and_reindeer_simulation

clean:
	rm -rf $(DIR_NAME)