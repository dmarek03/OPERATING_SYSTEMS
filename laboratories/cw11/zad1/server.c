#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#include "common.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    // Obsługa sygnału SIGINT
    signal(SIGINT, sigint_handler);

    // Przypisujemy do zmiennej ip_adress sformatowany do postaci uint32_t adres ip
    uint32_t ip_address = inet_addr(argv[1]);
    // Konwertujemy port podany w postaci long na postać uint16_t
    uint16_t port = (uint16_t)strtol(argv[2], NULL, 10);


    // Tworzymy strukturę opisującą adress naszego gniazda
    struct sockaddr_in addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Tworzymy gniazdo z następującymi atrybutami:
    // * domena - AF_INET
    // * tryb komunikacji:
    //         + SOCK_STREAM - niezawodna, uporządkowana, dwukierunkowa komunikacja strumieniowa oparta o połączenia;
    //         + SOCK_NONBLOCK - gniazdo w trybie nieblokującym
    // * protokół - 0 (domyślny)
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    int t = 1;
    // Ustawiamy opcje charakterystyczne dla gniazda:
    // * sockfd  -numer deskryptora gniazda
    // * level  -poziom ustawień, standardowo SOL_SOCKET
    // * optname - "nazwa opcji", czyli stała powiązana z danym parametrem
    // * optval - wskaźnik do zaalokowanego obszaru pamięci do którego ma trafić odczytana wartość opcji
    // * optlen — wskaźnik do zmiennej określającej zaalokowaną wielkość optval
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));

    // Wiążemy gniazdo z jego nazwą(adresem):
    // *sockfd — numer deskryptora gniazda
    // * addr - wskaźnik do struktury definiującej adres , z którym ma gniazdo ma zostac powiązane
    // * addrlen - wielkość struktury przekazanej w drugim argumencie
    if(bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        perror("bind");

    // Rozpoczęcie akceptowania połączeń od klientów(dla gniazd connection-oriented[SOCK_STREAM])
    // * sockfd - numer dekryptora gniazda
    // backlog -  maksymalna ilość połączeń, które mogą oczekiwać na zaakceptowanie
    if(listen(socket_fd, MAX_CLIENTS) < 0)
        perror("listen");

    // Inicializujemy tablicę przechowującą numery deskryptorów dla klientów

    int clients_fd_array[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients_fd_array[i] = -1;


    // Inincializujemy zbiór oraz tablicę przechowującą id clientów oraz tablicę przechowującą czas aktywności kilientów
    bool clients_id_set[MAX_CLIENTS] = {0};
    char clients_id_array[MAX_CLIENTS][MAX_CLIENT_ID_LEN] = {0};
    clock_t clients_alive_timeout[MAX_CLIENTS];

    clock_t ping_time = clock();

    while (!should_close) {
        int client_fd;
        //Jeśli udało się zaakceptować oczekujące na gnieździe połączenie, to rejestrujemy klienta na pierwszym wolnym
        // miejscu i ustawiamy jego alive_timeout jak bieżący czas
        if((client_fd = accept(socket_fd, NULL, 0)) > 0) {
            int i = 0;
            while (i < MAX_CLIENTS){
                if (clients_fd_array[i] == -1) {
                    clients_fd_array[i] = client_fd;
                    clients_alive_timeout[i] = clock();

                    printf("Client accepted at index: %d\n", i);
                    break;
                }

                i++;
            }

            if (i == MAX_CLIENTS)
                printf("Client limit reached\n");

        }
        // Dla klientów z aktywnym połączeniem probujemy odebrać wiadomość w sposób nieblokujący i zapisać ją w buforze message
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients_fd_array[i] == -1)
                continue;

            requested_message_t message;
            if(recv(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT) > 0) {
                switch(message.request_type) {
                    case TOALL:
                        printf("TO_ALL: %s FROM: %s \n", message.payload.to_all, message.sender_client_id);
                        for (int j = 0; j < MAX_CLIENTS; j++)
                            if (clients_fd_array[j] != -1 && i != j)
                                send(clients_fd_array[j], &message, sizeof(message), MSG_DONTWAIT);
                        break;

                    case TOONE:
                        printf("TO_ONE: %s %s FROM: %s \n", message.payload.to_one.target_client_id, message.payload.to_one.message, message.sender_client_id);
                        for (int i = 0; i < MAX_CLIENTS; i++)
                            if (clients_fd_array[i] != -1 && strncmp(clients_id_array[i], message.payload.to_one.target_client_id, MAX_CLIENT_ID_LEN) == 0)
                                send(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT);
                        break;

                    case LIST:
                        printf("LIST FROM: %s\n", message.sender_client_id);
                        int length = 0;
                        for (int j = 0; j < MAX_CLIENTS; j++)
                            if (clients_fd_array[j] != -1){
                                length++;
                                strncpy(message.payload.list.identifiers_list[j], clients_id_array[j], MAX_CLIENT_ID_LEN);
                            }
                        message.payload.list.list_lengths = length;

                        send(clients_fd_array[i], &message, sizeof(message), MSG_DONTWAIT);
                        break;

                    case ALIVE:
                        printf("ALIVE FROM: %s\n", message.sender_client_id);
                        clients_alive_timeout[i] = clock();
                        if (!clients_id_set[i])
                            strncpy(clients_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN);
                        break;

                    case STOP:
                        printf("STOP FROM: %s\n", message.sender_client_id);
                        clients_fd_array[i] = -1;
                        clients_id_set[i] = false;
                        break;
                }

                fflush(stdout);
            }
        }

        // Co 1 sekdunę Wysyłamy wiadomość ALIVE do wszystkich klientów, aby sprawdzić czy są aktywni
        if ((clock() - ping_time) / CLOCKS_PER_SEC > 1) {
            requested_message_t alive_message = {
                    .request_type = ALIVE
            };
            for (int i = 0; i < MAX_CLIENTS; i++)
                if (clients_fd_array[i] != -1)
                    send(clients_fd_array[i], &alive_message, sizeof(alive_message), MSG_DONTWAIT);
            ping_time = clock();
        }

        // Sprawdzamy czy klienci nie przekroczyli limitu czasu połączenia
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients_fd_array[i] != -1 && (clock() - clients_alive_timeout[i]) / CLOCKS_PER_SEC > 5) {
                printf("Client %s timed out\n", clients_id_array[i]);
                close(clients_fd_array[i]);
                clients_fd_array[i] = -1;
                clients_id_set[i] = false;
            }
    }
    // Zamknięcie wszystkich aktwynych połączęń serwera
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients_fd_array[i] != -1)
            close(clients_fd_array[i]);

    return 0;
}