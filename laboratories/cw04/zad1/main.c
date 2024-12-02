 #include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int arg, char **argv) {
    if (arg < 2) {
        fprintf(stderr, "At least one argument must be passed\n");
        return 1;
    }

    int n;
    // Checking if correct number was passed as argument
    if (sscanf(argv[1], "%d", &n) != 1) {
        fprintf(stderr, "Invalid number\n");
        return 1;
    }
    for (int i =0; i < n ; i++){
        // Jesli jesteśmy w procesie potomnym to wypsiujemy PPID i PID
        if(fork() == 0){
            printf("Parent process ID PPID: %d\nProcess id PID: %d\n", getppid(), getpid());

            exit(EXIT_SUCCESS);
        }
    }
    // Czekamy aż wszytskie procesy potomne się zakończą i wypisujemy liczbę procesów potomnych, którą należało utworzyć
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }
    printf("argv[1]: %d\n", n);





    return 0;
}
