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
#define MAX_PRINTERS 50
#define  SHARED_MEMORY_DESCRIPTOR_NAME  "printers_shared_memory"

volatile bool should_close = false;


void signal_handler(int signum){
    should_close = true;
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
    if (argc < 2){
        printf("Usage :%s <number of printers>\n", argv[0]);
        return -1;
    }

    // Konwersja liczby drukarek
    long number_of_printers = strtol(argv[1], NULL, 10);

    if (number_of_printers > MAX_PRINTERS){
        printf("Number of printers is too big, choose number less then %d\n", MAX_PRINTERS);
    }

    // Próba utworzenia i otworzenia deskryptora dla współdzielonego obszaru pamięci przez drukarki
    int memory_fd = shm_open( SHARED_MEMORY_DESCRIPTOR_NAME , O_RDWR|O_CREAT, S_IWUSR|S_IRUSR);

    if(memory_fd < 0){
        perror("Shared memory description opening error\n");
    }

    // Ustalamy rozmiar współdzielonej pamięci
    if(ftruncate(memory_fd, sizeof(memory_map_t)) < 0){
        perror("Ftruncante error\n");
    }
    // Próbujemy zmapowac obszar pamięci współdzielonej na adres pamięci programu
    memory_map_t* memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ|PROT_WRITE, MAP_SHARED, memory_fd, 0);

    if (memory_map == MAP_FAILED){
        perror("Memory mapping error \n");
    }
    // Czyścimy obszar pamięci współdzielonej
    memset(memory_map, 0, sizeof(memory_map_t));

    // Ustawiamy liczbę dostępnych drukarek
    memory_map->number_of_printers=number_of_printers;

    for(int sig =1 ;sig < SIGRTMAX; sig ++){
        signal(sig, signal_handler);
    }


    for(int i = 0; i < number_of_printers ;i ++){
        sem_init(&memory_map->printers[i].printer_semaphore, 1, 1);

        pid_t printer_pid = fork();
        if(printer_pid < 0 ){
            perror("Fork error\n");
            return -1;
        } else if(printer_pid ==0){

            while (!should_close){
                // Jeśli otrzymano zlecenie wydruku rozpoczynamy proces odczytu i drukowania danych
                if(memory_map->printers[i].printer_state ==PRINTING){
                    for(int j= 0 ; j < memory_map ->printers[i].printer_buffer_size; j ++){
                        printf("%c", memory_map->printers[i].printer_buffer[j]);
                        sleep(1);
                    }
                    printf("\n");
                    //Przekierowujemy bufforowane dane na standardowe wyjście
                    fflush(stdout);
                    // Po wykonaniu zadania stanu drukarki ustawiamy jak oczekiwanie
                    memory_map ->printers[i].printer_state = WAITING;
                    // Inkrementujemy semafor w celu sygnalizacji wykonania zadania przez drukarkę
                    sem_post(&memory_map->printers[i].printer_semaphore);
                }
            }
            exit(0);
        }

    }

    while(wait(NULL) > 0 ){}

    // Niszczenie semaforów
    for(int i = 0 ; i < number_of_printers ; i ++){
        sem_destroy(&memory_map->printers[i].printer_semaphore);
    }
    // Czyszczenie obszar pamięci współdzielonej
    munmap(memory_map, sizeof(memory_map_t));

    // Zamknięcie deskryptora dla współdzielonego obszaru pamięci
    shm_unlink(SHARED_MEMORY_DESCRIPTOR_NAME);


}