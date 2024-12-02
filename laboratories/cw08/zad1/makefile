CC = gcc
FLAGS = -Wall
DIR_NAME = build

#.PHONY = clean run_clients_and_printers clients_and_printers build
#
#clients_and_printers: main.c
#	$(CC) $(FLAGS) -c main.c -o $(DIR_NAME)/main.o
#	$(CC) $(FLAGS) $(DIR_NAME)/main.o -o $(DIR_NAME)/clients_and_printers
#
#
#build:clean
#	mkdir -p $(DIR_NAME)
#	make clients_and_printers
#
#run_clients_and_printers:build
#	$(DIR_NAME)/clients_and_printers
#
#
#clean:
#	rm -rf $(DIR_NAME)

.PHONY: all clean



all: printers_system user_simulator

user_simulator: user_simulator.c
	rm -rf /dev/mqueue/*
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) -o $(DIR_NAME)/$@ $< -lrt -lpthread


printers_system:printers_system.c
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) -o $(DIR_NAME)/$@ $< -lrt -lpthread


run_user_simulator: user_simulator
	./$(DIR_NAME)/user_simulator 10


run_printers_system: printers_system
	./$(DIR_NAME)/printers_system 5


clean:
	rm -rf $(DIR_NAME)