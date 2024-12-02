#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <libgen.h>

void sigusr1_handler(int signum) {
    printf("Received SIGUSR1\n");
}

void create_mask(){
    // sigset_t -> struktura przechowująca dane o sygnałach
    sigset_t mask;
    // Inicjujemy pusty zestaw sygnałów 'mask'
    sigemptyset(&mask);
    // Dodanie SIGUSR1 do naszego zestawu -> sygnał SIGUSR1 będzie blokowany, gdy zestaw zostanie użyty jako maska
    sigaddset(&mask, SIGUSR1);
    //Ustawienie mask jako bieżącą maskę sygnałów dla procesu
    // SIG_BLOCK -> zestaw sygnałów podany jak drugi parametr będzie dodany do obecnej maski => sygnały z zestawu będą blokowane
    // &mask -> adres sygnałów, które chcemy dodać do obcnej maski
    // NULL -> nie chcemy pobierac maski do zapisania w innym miejscu
    sigprocmask(SIG_SETMASK, &mask, NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "none") != 0 && strcmp(argv[1], "ignore") != 0 && strcmp(argv[1], "handler") != 0 && strcmp(argv[1], "mask") != 0)) {
        printf("Pass command '%s' should be one of them:  [none|ignore|handler|mask]\n", argv[1]);
        return 1;
    }

    char *option = argv[1];

    switch(option[0]) {
        case 'n':
            printf("No action at signal\n");
            // Powiadomienie jądra systemu o obsłudze sygnału podanego jako pierwszy paramater za pomocą funkcji z drugiego paramteru
            signal(SIGUSR1, SIG_DFL);
            break;
        case 'i':
            signal(SIGUSR1, SIG_IGN);
            break;
        case 'h':
            signal(SIGUSR1, sigusr1_handler);
            break;
        case 'm':
            create_mask();
            break;
        default:
            fprintf(stderr, "Invalid option was passed");
            return 1;

    }

    if(strcmp(option, "none")!=0){
        raise(SIGUSR1);
    }


    sigset_t pending_signals;
    sigemptyset(&pending_signals);
    // Pobranie zestawu sygnałów oczekujących dla bieżącego procesu oraz zapisanie ich w zmiennej `pending_signals`
    sigpending(&pending_signals);
    // Jeśli w zapisanym zestawie znajduje się `SIGUSR1` to wyświetlamy komunikat, że ten sygnał jest widoczny
    if (sigismember(&pending_signals, SIGUSR1)) {

        printf("Pending SIGUSR1 signal is visible\n");
    } else {
        printf("Pending SIGUSR1 signal is not visible\n");
    }

    return 0;
}
