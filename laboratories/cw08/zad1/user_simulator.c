#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <semaphore.h>
#define MAX_PRINTER_BUFFER_SIZE 256
#define MAX_PRINTERS 50
#define SHARED_MEMORY_DESCRIPTOR_NAME "printers_shared_memory"

volatile bool should_close = false;


void signal_handler(int signum){
    should_close = true;
}


void generate_random_string(char* buffer, int length){
    for(int i=0;i< length;i++){
        buffer[i] = 'a' + rand()%26;
    }
    buffer[length] = '\0';

}

typedef enum {
    WAITING = 0,
    PRINTING = 1
}printer_state_t;

typedef struct {
    sem_t printer_semaphore;
    char printer_buffer[MAX_PRINTERS];
    size_t printer_buffer_size;
    printer_state_t printer_state;
}printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    long number_of_printers;
}memory_map_t;


int main(int argc, char** argv){

    if(argc < 2){
        printf("Usage: %s <number_of_users>\n", argv[0]);
        return -1;
    }
    // Konwertujemy liczbę reprezentującą liczbę użytkowników będaych sysłać zadania do wydrukowania
    long number_of_users = strtol(argv[1], NULL, 10);

    // Otwieramy deskryptor do współedzielonego obszaru pamięci
    int memory_fd = shm_open(SHARED_MEMORY_DESCRIPTOR_NAME, O_RDWR, S_IRUSR| S_IWUSR);

    if(memory_fd <  0){
        perror("Shared memory opening error");
    }
    // Próbujemy zmapowac obszar pamięci współdzielonej z dostępem do odczytu i zapisu
    memory_map_t* memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ|PROT_WRITE, MAP_SHARED, memory_fd, 0);
    if(memory_map == MAP_FAILED){
        perror("Shared memory mapping error\n");
    }

    // Tworzmy bufor do przechowywania wiadomości wysyłanych przez użytkowników do drukowania
    char user_buffer[MAX_PRINTER_BUFFER_SIZE] = {0};

    for(int sig = 1 ; sig < SIGRTMAX;sig++){
        signal(sig, signal_handler);

    }
    int seed = 0;
    for(int i = 0; i < number_of_users;i++){
        seed += 10;
        pid_t user_pid = fork();
        if(user_pid < 0){
            perror("Fork error\n");
            return  -1;
        }else if(user_pid == 0){
            srand(seed);
            while (!should_close){
                generate_random_string(user_buffer, 10);

                int printer_idx =-1;
                for(int j = 0; j < memory_map->number_of_printers;j++){
                    int val;
                    // Odczytyujemy aktualną wartość semafora
                    sem_getvalue(&memory_map->printers[j].printer_semaphore, &val);
                    if(val >  0 ){
                        printer_idx = j;
                        break;
                    }
                }
                // Jeśli wszytskie drukarki są zajęte wyślij żądanie wydruku do losowo wybranej drukarki
                if(printer_idx == -1){
                    printer_idx = rand() % memory_map->number_of_printers;
                }
                // Próbujemy dekrementowac semafor, blokujemy program jeśli drukara obecnie jest zajęta
                if(sem_wait(&memory_map->printers[printer_idx].printer_semaphore) < 0){
                    perror("Sem waiting error\n");
                }
                // Kopiujemy dane do wydruku do bufora
                memcpy(memory_map->printers[printer_idx].printer_buffer, user_buffer, MAX_PRINTER_BUFFER_SIZE);
                memory_map->printers[printer_idx].printer_buffer_size= strlen(user_buffer);

                // Aktualziujemy stan drukarki na drukowanie
                memory_map->printers[printer_idx].printer_state = PRINTING;

                printf("User %d is printing on printer %d \n", i, printer_idx);
                fflush(stdout);
                sleep(rand()% 3 + 1);
            }
            exit(0);

        }

    }

    while(wait(NULL) > 0){};

    munmap(memory_map, sizeof(memory_map_t));


}