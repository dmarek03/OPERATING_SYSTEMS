#include "mode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


enum Mode mode;
int request_count = 0;
int pid;
// siginfo_t* inf -> wskaźnik do struktury siginfo_t, która zawiera szczegółowe informacje o procesie
void handle_signal(int signo, siginfo_t* info, void* context) {
    // Przypisanie  identyfikatora procesu (PID) wysyłającego sygnał do zmiennej pid
    pid = info->si_pid;
    // Przypisanie statusu sygnału do zmiennej value
    int value = info->si_status;

    if (value >= 1 && value <= 3) {
        request_count += 1;
        mode = value;
    } else {
        fprintf(stderr, "Invalid status [%d]\n", value);
    }
    // Wysyłanie sygnału do procesu o podanym pid
    kill(pid, SIGUSR1);
}

void set_sigaction() {
    // Tworzymy strukturę do obsługi synałów z funkcją handle_signal jak wskażnik do naszych sygnałów
    struct sigaction signal_action;
    signal_action.sa_sigaction = handle_signal;
    // Ustawiamy flagę sa_flags na SA_SGINFO, co umożliwa nam otrzymywanie dodatkowych informacji o przechwytywanym sygnale,
    // m.in, takich jak numer sygnału, identyfikator procesu wysyłającego sygnał, czas jego wysłania,
    signal_action.sa_flags = SA_SIGINFO;
    // Czyścimy zbiór złożonych z maski sygnałów naszego sygnału wejściowego
    sigemptyset(&signal_action.sa_mask);
    // Obsługa sygnału SIGUSR1 przez skonfigurowaną wyżej strukturę signal_action
    sigaction(SIGUSR1, &signal_action, NULL);
}

void print_numbers(int from, int to) {
    for (int i = from; i <= to; i++)
        printf("%d ", i);
    printf("\n");
}

void print_requests() {
    printf("Number of all requests: %d\n", request_count);
}



void close_p() {
    printf("Exiting the catcher\n");
    // Dane w buforze wyjścia zostaną wypisane bez czekania na zakończenie programu
    fflush(stdout);
    exit(0);
}

int main(int argc, char **argv) {
    printf("Catcher (PID: %d)\n", getpid());
    printf("Waiting for signals...\n\n");

    set_sigaction();


    while (1) {
        //Zawieszamy wywołujący proces aż do chwili otrzymania dowolnego sygnału
        pause();
        switch (mode) {
            case PRINT_NUMS:
                print_numbers(1, 100);
                break;
            case PRINT_CHANGES:
                print_requests();
                break;
            case EXIT:
                close_p();
                return 0;
        }

    }

}