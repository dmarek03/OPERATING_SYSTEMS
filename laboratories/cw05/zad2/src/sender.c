#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int success = 0;

void handle_signal(int signo) {
    success = 1;
}

void set_sigaction() {
    // Używamy struktury sigaction do niezawodnej osbługi sygnałów
    struct sigaction signal_action;
    // Jak wskaźnik do naszej funkjci sygnałów ustawiamy funkcję handle_signal
    signal_action.sa_handler = handle_signal;
    // Wyczyszczeni zbioru blokowanych sygnałów (maski sygnałów z naszego signal action)
    sigemptyset(&signal_action.sa_mask);
    // Obsługa sygnału SIGUSR1 przez skonfigurowaną wyżej strukturę signal_action
    sigaction(SIGUSR1, &signal_action, NULL);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        perror("[ERROR] At least 2 arguments are required\n");
        return -1;
    }

    int pid = atoi(argv[1]);

    printf("Sender (PID: %d)\n", getpid());
    printf("Catcher's PID: %d\n", pid);

    for (int i = 2; i < argc; i++) {
        const int mode = atoi(argv[i]);
        success = 0;

        set_sigaction();

        // Tworzymy strukturę sigval_t, która umożliwa przesłanie dodatkowych danych wraz z sygnałem, w naszym przypadku
        // inicjalizjuemy ją wartością mode
        sigval_t sigval = {mode};
        // Wysyłamy sygnał SIGUSR1 wraz z dodatkową informacją sigval do procesu o podanym PID
        sigqueue(pid, SIGUSR1, sigval);
        printf("Sent [mode %d]\n", mode);

        while (!success);

        printf("Received confirmation from catcher [mode %d]\n", mode);

    }

    return 0;
}
