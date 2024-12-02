CC = gcc
FLAGS = -Wall
DIR_NAME = build

.Phony = client server

client: client.c
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS)  -o $(DIR_NAME)/$@ $<



server: server.c
	rm -rf /dev/mqueue/*
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) -o $(DIR_NAME)/$@ $<



run_client: client
	./$(DIR_NAME)/client


run_server: server
	./$(DIR_NAME)/server



clean:
	rm -rf $(DIR_NAME)
	rm -f /dev/mqueue/*