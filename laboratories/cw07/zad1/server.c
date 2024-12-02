#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>

#define MESSAGE_BUFFER_SIZE 2048
#define MAX_CLIENT_NUMBER 3
#define SERVER_QUEUE_NAME "/basic_server_queue"
typedef enum{
    INIT,
    IDENTIFIER,
    MESSAGE_TEXT,
    CLIENT_CLOSE
}message_type_t;

typedef struct {
    message_type_t  type;
    int identifier;
    char text[MESSAGE_BUFFER_SIZE];
}message_t;

volatile bool is_working = false;


void signal_handler(int signum){
    is_working = true;
}

int main() {
    struct mq_attr attributes = {
            .mq_flags = 0,
            .mq_msgsize = sizeof(message_t),
            .mq_maxmsg = 10
    };

    // Stworzenie serwera
    mqd_t mq_descriptor = mq_open(SERVER_QUEUE_NAME, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR, &attributes);
    if(mq_descriptor < 0){
        perror("mq_descriptor opening error\n");
    }
    message_t received_message;
    // Inicializacja kolejki klientów
    mqd_t clients[MAX_CLIENT_NUMBER];
    for(int i = 0;i < MAX_CLIENT_NUMBER ;i ++){
        clients[i] = -1;
    }

    for(int sig = 1;sig < SIGRTMAX; sig ++){
        signal(sig, signal_handler);
    }
    int id;
    while (!is_working){
        // Odbieranie wiadomosci z kolejki sewera
        mq_receive(mq_descriptor, (char *)&received_message, sizeof(received_message), NULL);
        switch (received_message.type) {
            case INIT:
                id = 0;
                while(clients[id]!=-1 && id < MAX_CLIENT_NUMBER){
                    id++;
                }
                if(id == MAX_CLIENT_NUMBER){
                    printf("Max number of clients has been reached\n");
                    continue;
                }
                clients[id] = mq_open(received_message.text, O_RDWR, S_IRUSR|S_IWUSR, NULL);
                if(clients[id] < 0){
                    perror("Opening queue error\n");
                }
                // Wysłanie wiadomości incjującej do klienta
                message_t send_message = {
                        .type = IDENTIFIER,
                        .identifier = id
                };

                mq_send(clients[id], (char *)&send_message, sizeof(send_message), 10);
                printf("New client was registered at id:%d\n", id);
                break;
            case MESSAGE_TEXT:

               for(int client = 0 ; client < MAX_CLIENT_NUMBER ;client ++){
                   if(client != received_message.identifier && client != -1){
                       mq_send(clients[client], (char*)&received_message, sizeof(received_message), 10);
                   }
               }
               break;

            case CLIENT_CLOSE:
                mq_close(clients[received_message.identifier]);
                clients[received_message.identifier] = -1;
                printf("Connection closed with client at id :%d\n", received_message.identifier);
                break;
            default:
                printf("Unrecognized message type : %d \n", received_message.type);
                break;
        }
    }
    printf("Exiting sever\n");
    // Zamykamy kolejki dla wszystkich klientów
    for(int i = 0 ; i < MAX_CLIENT_NUMBER; i++){
        if(clients[i]!=-1){
            mq_close(clients[i]);
        }
    }
    // Zamykamy serwer
    mq_close(mq_descriptor);
    mq_unlink(SERVER_QUEUE_NAME);

}


