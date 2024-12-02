CC= gcc
CFLAGS = -Wall
DIR_NAME= build


.PHONY: clean

client: client.c
	mkdir -p $(DIR_NAME)
	$(CC) $(CFLAGS) -o $(DIR_NAME)/$@ $<


server: server.c
	mkdir "-p" $(DIR_NAME)
	$(CC) $(CFLAGS) server.c -o $(DIR_NAME)/server



run_client:client
	./$(DIR_NAME)/client $(NAME) 127.0.0.1 4001


run_server: server
	./$(DIR_NAME)/server 127.0.0.1 4001


clean:
	rm -rf $(DIR_NAME)
