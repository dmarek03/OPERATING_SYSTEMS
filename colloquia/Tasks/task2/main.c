#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
int pid;
void sighandler(int signo, siginfo_t* info, void* context) {
    pid = info->si_pid;
    int value = info->si_status;
    printf("Value recived from parent %d\n" ,value );

}



int main(int argc, char* argv[]) {

    if(argc != 3){
        printf("Not a suitable number of program parameters\n");
        return 1;
    }


    struct sigaction action;
    action.sa_sigaction = &sighandler;
    action.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &action, NULL);

    //..........


    int child = fork();
    if(child == 0) {
        //zablokuj wszystkie sygnaly za wyjatkiem SIGUSR1
        //zdefiniuj obsluge SIGUSR1 w taki sposob zeby proces potomny wydrukowal
        //na konsole przekazana przez rodzica wraz z sygnalem SIGUSR1 wartosc
        sigset_t set;
        sigfillset(&set);
        sigdelset(&set, SIGUSR1);
        sigprocmask(SIG_SETMASK, &set, NULL);
        pause();
    }
    else {
        //wyslij do procesu potomnego sygnal przekazany jako argv[2]
        //wraz z wartoscia przekazana jako argv[1]
        // tworzymy strukturę umożliwającą przekazanie  dodatkowych danych wraz z sygnałem
        sigval_t sigval = {atoi(argv[1])};

        //Wysyłałmy do procesu potomnego sygnał o identfikatorze argv[2] z dodatkową informacją z argv[1]
        sigqueue(child, atoi(argv[2]), sigval);
    }

    return 0;
}


