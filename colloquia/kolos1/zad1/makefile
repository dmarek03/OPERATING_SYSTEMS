CC = gcc
FLAGS = -Wall
DIR_NAME = build
LIB_NAME = libbibl1

.PHONY: shared_lib dynamic_load_zad1 test

# Biblioteka współdzielona:
# -fPIC -> pozwala generowac kod, który może być przenoszony i używany w dowolnym
#         miejscu w pamięci, niezależenie od konkretnej lokalizacji.Pozwala to na ładowanie bibliotek współdzieonych
#         w różne miejsca pamięci dla różnych programów
# -shared -> informuje, że biblioteka będzie współdzielona
shared_lib:
	$(CC) $(FLAGS) -fPIC -c bibl1.c -o $(DIR_NAME)/bibl1.o
	$(CC) -shared $(DIR_NAME)/bibl1.o -o $(DIR_NAME)/$(LIB_NAME).so

# FLaga -ldl: -> wskazuje,że korzystamy z bibloteki libdl.so, która zwiera funkcję związane z dynamicznym ładowaniem
dynamic_load_zad1:
	$(CC) $(FLAGS) zad1.c -o $(DIR_NAME)/test_zad1 -ldl


build:clean
	mkdir -p $(DIR_NAME)
	make shared_lib;
	make dynamic_load_zad1


test:build
	./$(DIR_NAME)/test_zad1


clean:
	rm -rf $(DIR_NAME)