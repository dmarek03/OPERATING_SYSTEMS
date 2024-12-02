CC = gcc
FLAGS = -Wall
DIR_NAME = build

sender:
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) -c zad3_nad.c -o $(DIR_NAME)/zad3_nad.o
	$(CC)  $(DIR_NAME)/zad3_nad.o -o $(DIR_NAME)/sender


catcher:
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) -c zad3_odb.c -o $(DIR_NAME)/zad3_odb.o
	$(CC)  $(DIR_NAME)/zad3_odb.o -o $(DIR_NAME)/catcher


run_catcher: catcher
	./$(DIR_NAME)/catcher


run_sender: sender
	./$(DIR_NAME)/sender

clean:
	rm -rf $(DIR_NAME)



