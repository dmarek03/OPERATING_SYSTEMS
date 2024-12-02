CC = gcc
LIB_NAME = libcollatz

.PHONY: static shared_so static_client shared_so_client shared_dlopen_client clean
static:
		$(CC) -c collatz.c
		ar rcs $(LIB_NAME).a collatz.o

# ar - archiwizator
# r - dodanie pliku do archiwum
# c - utworzenie archiwum jeśli nie istnieje
# s - aktualizacja indeksu archiwum


shared_so:
		$(CC)  -fPIC -c collatz.c
		$(CC)  -shared collatz.o -o $(LIB_NAME).so

# fPIC -  force Position Independent Code - pozwala generowac kod, który może być przenoszony i używany w dowolnym
#         miejscu w pamięci, niezależenie od konkretnej lokalizacji.Pozwala to na ładowanie bibliotek współdzieonych
#         w różne miejsca pamięci dla różnych programów
# shared - wskazuje, że utworzona biblioteka będzie współdzielona

static_client: client.c
		$(CC)  client.c $(LIB_NAME).a -o static_client


shared_so_client: client.c
		$(CC)  client.c -L. -lcollatz -Wl,-rpath=. -o shared_so_client

# -L. - przeszukanie bieżącego katalogu w celu znalezienia biblioteki
# -lcollatz - flaga l wskazuje ,że będziemy używać biblioteki libcollatz.so
# -Wl,-rpath=. - dodaje ścieżkę do katalogu, w którym program będzie szukał bibiloteki w tym przypadku bieżący katalog



shared_dlopen_client: client.c
		$(CC) client.c -o shared_dlopen_client -ldl -D DYNAMIC_DLOPEN

# -ldl - wskazuje, że program korzysta z biblioteki libdl.so, która zawiera funkcję związane z dynamicznym ładowaniem,
#        takie jak, "dlopen" czy "dlsym"

clean:
		rm -f  collatz.o $(LIB_NAME).a $(LIB_NAME).o $(LIB_NAME).so static_client shared_so_client shared_dlopen_client