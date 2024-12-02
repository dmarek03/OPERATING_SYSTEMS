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


    signal(SIGINT, sigint_handler);


    uint32_t ip_address = inet_addr(argv[1]);
    uint16_t port = (uint16_t)strtol(argv[2], NULL, 10);


    struct sockaddr_in addr = {
            .sin_addr.s_addr = ip_address,
            .sin_port = htons(port),
            .sin_family = AF_INET
    };


    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror("socket");
        return -1;
    }


    int t = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(int));


    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }


    int clients_fd_array[MAX_CLIENTS] = {-1};
    bool clients_id_set[MAX_CLIENTS] = {0};
    char clients_id_array[MAX_CLIENTS][MAX_CLIENT_ID_LEN] = {0};
    struct sockaddr_in clients_addr[MAX_CLIENTS];
    clock_t clients_alive_timeout[MAX_CLIENTS];

    for (int i = 0; i < MAX_CLIENTS; i++)
        clients_fd_array[i] = -1;

    clock_t ping_time = clock();
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    while (!should_close) {

        requested_message_t message;
        int recv_len = recvfrom(socket_fd, &message, sizeof(message), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (recv_len > 0) {
            switch (message.request_type) {
                case TOALL:
                    printf("TO_ALL: %s FROM: %s \n", message.payload.to_all, message.sender_client_id);
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients_fd_array[i] != -1) {
                            sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr*)&clients_addr[i], client_addr_len);
                        }
                    }
                    break;

                case TOONE:
                    printf("TO_ONE: %s %s FROM: %s \n", message.payload.to_one.target_client_id, message.payload.to_one.message, message.sender_client_id);
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients_fd_array[i] != -1 && strncmp(clients_id_array[i], message.payload.to_one.target_client_id, MAX_CLIENT_ID_LEN) == 0) {
                            sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr*)&clients_addr[i], client_addr_len);
                        }
                    }
                    break;

                case LIST:
                    printf("LIST FROM: %s\n", message.sender_client_id);
                    int length = 0;
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clients_fd_array[j] != -1) {
                            length++;
                            strncpy(message.payload.list.identifiers_list[j], clients_id_array[j], MAX_CLIENT_ID_LEN);
                        }
                    }
                    message.payload.list.list_lengths = length;
                    sendto(socket_fd, &message, sizeof(message), 0, (struct sockaddr*)&client_addr, client_addr_len);
                    break;

                case ALIVE:
                    printf("ALIVE FROM: %s\n", message.sender_client_id);
                    bool found = false;
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (clients_fd_array[i] != -1 && strncmp(clients_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN) == 0) {
                            clients_alive_timeout[i] = clock();
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        for (int i = 0; i < MAX_CLIENTS; i++) {
                            if (clients_fd_array[i] == -1) {
                                clients_fd_array[i] = 1;
                                clients_alive_timeout[i] = clock();
                                strncpy(clients_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN);
                                clients_addr[i] = client_addr;
                                break;
                            }
                        }
                    }
                    break;

                case STOP:
                    printf("STOP FROM: %s\n", message.sender_client_id);
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (strncmp(clients_id_array[i], message.sender_client_id, MAX_CLIENT_ID_LEN) == 0) {
                            clients_fd_array[i] = -1;
                            clients_id_set[i] = false;
                            break;
                        }
                    }
                    break;
            }
        }


        if ((clock() - ping_time) / CLOCKS_PER_SEC > 1) {
            requested_message_t alive_message = {.request_type = ALIVE};
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients_fd_array[i] != -1) {
                    sendto(socket_fd, &alive_message, sizeof(alive_message), 0, (struct sockaddr*)&clients_addr[i], client_addr_len);
                }
            }
            ping_time = clock();
        }


        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients_fd_array[i] != -1 && (clock() - clients_alive_timeout[i]) / CLOCKS_PER_SEC > 5) {
                printf("Client %s timed out\n", clients_id_array[i]);
                clients_fd_array[i] = -1;
                clients_id_set[i] = false;
            }
        }
    }

    close(socket_fd);
    return 0;
}
