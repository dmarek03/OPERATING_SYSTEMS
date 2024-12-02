CC = gcc -Wall
FLAGS = -Wall
SRC_DIR = src
BUILD_DIR = build
CATCHER_NAME = catcher
SENDER_NAME = sender


build: clean
	mkdir -p $(BUILD_DIR)
	make catcher;
	make sender

catcher:
	$(CC) $(FLAGS) -c $(SRC_DIR)/$(CATCHER_NAME).c -o $(BUILD_DIR)/$(CATCHER_NAME).o
	$(CC) $(FLAGS) $(BUILD_DIR)/$(CATCHER_NAME).o -o $(BUILD_DIR)/$(CATCHER_NAME)

sender:
	$(CC) $(FLAGS) -c $(SRC_DIR)/$(SENDER_NAME).c -o $(BUILD_DIR)/$(SENDER_NAME).o
	$(CC) $(FLAGS) $(BUILD_DIR)/$(SENDER_NAME).o -o $(BUILD_DIR)/$(SENDER_NAME)


test: build
	$(BUILD_DIR)/catcher

clean:
	rm -rf $(BUILD_DIR)
