#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "common.h"

volatile bool should_close = false;

void sigint_handler(int signo) {
    should_close = true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <ip> <port> <self identifier> \n", argv[0]);
        return -1;
    }

    signal(SIGINT, sigint_handler);

    // Odpowiednio konwertujemy argumenty wejściowe

    char* client_identifier = argv[1];
    uint32_t ip_address = inet_addr(argv[2]);
    uint16_t port = (uint16_t)strtol(argv[3], NULL, 10);


    // Tworzymy strukturę opisującą adress naszego gniazda
    struct sockaddr_in addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };

    // Tworzymy gniazdo z odpowiednimi atrybutami
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);


    // Próbujemy się połączyć z serwerem
    if(connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        perror("Connection error");


    // Jeśli udało się nawiązać połączenie wysyłamy wiadomość ALIVE do serwera, aby poinformowac o swojej obecności
    requested_message_t alive_message = {
            .request_type = ALIVE
    };
    strncpy(alive_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);
    send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);

    // Tworzymy proces do nasłuchiwania wiadomości od serwera
    pid_t listener_pid = fork();
    if (listener_pid < 0)
        perror("fork listener");
    // Proces potomny odbiera wiadomośći od serwera i przetwarza je ze względu na rodzaj otrzymanej wiadomości
    else if (listener_pid == 0) {
        while (!should_close) {
            requested_message_t message;
            recv(socket_fd, &message, sizeof(message), MSG_WAITALL);
            switch(message.request_type) {
                case LIST:
                    for (int i = 0; i < message.payload.list.list_lengths; i++) {
                        printf("%s\n", message.payload.list.identifiers_list[i]);
                    }
                    break;
                case TOALL:
                    printf("TOALL FROM: %s: %s\n", message.sender_client_id, message.payload.to_all);
                    break;
                case TOONE:
                    printf("TOONE FROM: %s: %s\n", message.sender_client_id, message.payload.to_one.message);
                    break;
                case ALIVE:
                    send(socket_fd, &alive_message, sizeof(alive_message), MSG_DONTWAIT);
                    break;
                default:
                    printf("Invalid response type! \n");
                    break;
            }
        }

    // Proces główny oczekuje na wpisanie wiadomośći od użytkownika
    // Dla każdego obsługiwanego typu żądania tworzy odpowiednią wiadomość i wysyłą ją do serwera
    } else {
        char* request_type_input_buffer = NULL;
        while (!should_close) {
            if(scanf("%ms", &request_type_input_buffer) == 1) {
                requested_message_t message;
                strncpy(message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);

                if (strncmp(request_type_input_buffer, "LIST", 4) == 0){
                    message.request_type = LIST;

                    if(send(socket_fd, &message, sizeof(message), MSG_DONTWAIT) < 0)
                        printf("Sending error");
                } else if (strncmp(request_type_input_buffer, "2ALL", 4) == 0) {
                    message.request_type = TOALL;
                    scanf("%s", message.payload.to_all);
                    send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
                } else if (strncmp(request_type_input_buffer, "2ONE", 4) == 0) {
                    message.request_type = TOONE;
                    scanf("%s", message.payload.to_one.target_client_id);
                    scanf("%s", message.payload.to_one.message);
                    send(socket_fd, &message, sizeof(message), MSG_DONTWAIT);
                } else {
                    printf("Invalid request type! \n");
                }

                free(request_type_input_buffer);
            } else
                perror("scanf input");

        }

        // Wysyłamy wiadomość STOP do serwera aby poinformowac o zakończeniu połączenia przez danego kilenta

        requested_message_t stop_message = {
                .request_type = STOP
        };
        strncpy(stop_message.sender_client_id, client_identifier, MAX_CLIENT_ID_LEN);
        send(socket_fd, &stop_message, sizeof(stop_message), MSG_DONTWAIT);
    }
    // Zamykamy deskryptor pliku socket_fd
    close(socket_fd);

    return 0;
}