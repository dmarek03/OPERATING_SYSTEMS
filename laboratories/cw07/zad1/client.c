#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

#define SERVER_QUEUE_NAME "/basic_server_queue"
#define CLIENT_QUEUE_NAME_SIZE 64
#define MESSAGE_BUFFER_SIZE 2048
#define MAX_CLIENT_NUMBER 3
#define MIN(x, y)(x < y ? x : y)
typedef enum{
    INIT,
    IDENTIFIER,
    MESSAGE_TEXT,
    CLIENT_CLOSE,
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
    pid_t pid = getpid();
    char  queue_name[CLIENT_QUEUE_NAME_SIZE] = {0};
    sprintf(queue_name,"/chat_client_queue_%d", pid);

    struct mq_attr attributes = {
            .mq_flags = 0,
            .mq_msgsize =sizeof(message_t),
            .mq_maxmsg = 10,

    };
    // Tworzymy kolejkę kilenta z następującym opcja dostepu:
    // * 0_RWDR - otwórz do zapisu i odczytu
    // * O_CREATE - stówrz kolejkę jeśli nie istnieje
    // * S_IRUSR | S_IWUSR - zezwól użytkownikowi na operacja odczytu i zapisu

    mqd_t mq_client_descriptor = mq_open(queue_name, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR, &attributes);
    if(mq_client_descriptor < 0){
        perror("client queue opening error\n");
    }
    mqd_t mq_server_descriptor = mq_open(SERVER_QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(mq_server_descriptor < 0){
        perror("server queue opening error\n");
    }

    // Przygotowanie struktury wiadomości inicjującej
    message_t init_message = {
            .type =INIT,
            .identifier = -1,
            };
    memcpy(init_message.text, queue_name, MIN(CLIENT_QUEUE_NAME_SIZE-1, strlen(queue_name) ) );
    if(mq_send(mq_server_descriptor, (char*)&init_message, sizeof(init_message), 10) < 0){
        perror("Sending init message error\n");

    }
    // Stworzenie połączenie pomiędzy procesem macierzystym (odpowiedzilanym za wysyłanie do serwera)
    // a potomnym(czytającym wiadomośći z serwera)
    int to_parent_pipe[2];
    if(pipe(to_parent_pipe) < 0){
        perror("Creating pipe error\n");
    }

    for(int sig=1; sig <SIGRTMAX; sig++){
        signal(sig, signal_handler);
    }

    pid_t listener_pid = fork();
    if(listener_pid < 0){
        perror("Creating listener error\n");
    }
    else if(listener_pid == 0) {
        // Zamykamy wejście odpowiedzialne za odczyt
        close(to_parent_pipe[0]);
        message_t received_message;


        while (!is_working) {
            // Odbieramy wiadomości do póki nie otrzymamy wiadomośći blokującej
            mq_receive(mq_client_descriptor, (char *) &received_message, sizeof(received_message), NULL);

            switch (received_message.type) {
                case IDENTIFIER:
                    printf("Client register at server with number:%d\n", received_message.identifier);
                    write(to_parent_pipe[1], &received_message.identifier, sizeof(received_message.identifier));
                    break;

                case MESSAGE_TEXT:
                    printf("Received from id: %d, message: %s\n", received_message.identifier, received_message.text);
                    break;


                default:
                    printf("Received unrecognised message type from server:%d\n", received_message.type);
                    break;
            }
        }
        printf("Exiting from receiver loop\n");
        exit(0);
    }else{
        // Zamykamy wyjście odpowiedzialane za zapis
        close(to_parent_pipe[1]);
        int identifier = -1;
        if(read(to_parent_pipe[0], &identifier, sizeof(identifier)) < 0){
            perror("Message reading error\n");
        }
        char * buffer  = NULL;
        while (!is_working){
            // Pobieramy atrybuty wiadomości
            mq_getattr(mq_server_descriptor, &attributes);

            if(attributes.mq_curmsgs >= attributes.mq_maxmsg){
                printf("Sever is busy, please wait\n");
                continue;
            }

            if(scanf("%ms", &buffer)==1){
                message_t send_message = {
                        .type = MESSAGE_TEXT,
                        .identifier = identifier,
                };
                memcpy(send_message.text, buffer, MIN(MESSAGE_BUFFER_SIZE-1, strlen(buffer)));
                // Wysyłamy wiadomość do serwera
                mq_send(mq_server_descriptor, (char*)&send_message, sizeof(send_message), 10);
                free(buffer);
                buffer = NULL;
            }
            else{
                perror("Scanning data error\n");
            }

        }
        printf("Exiting from sending loop\n");
        if(identifier != -1){
           // Przygotowanie powiadomienia,że klient wychodzi z serwera
            message_t message_close = {
                    .type = CLIENT_CLOSE,
                    .identifier = identifier
            };
            // Powiadamiamy serwer o wyjściu klienta z serwera
            mq_send(mq_server_descriptor, (char*)&message_close, sizeof(message_close), 10);
        }

        mq_close(mq_server_descriptor);
        mq_close(mq_client_descriptor);

        mq_unlink(queue_name);
    }

}
