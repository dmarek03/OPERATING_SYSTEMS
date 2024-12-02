CC = gcc
CFLAGS = -Wall
DIR_NAME = build
all: life

life: clean
	mkdir -p $(DIR_NAME)
	$(CC) $(CFLAGS) grid.c life.c -o $(DIR_NAME)/life -lncursesw -lpthread -lm

clean:
	rm -rf $(DIR_NAME)
