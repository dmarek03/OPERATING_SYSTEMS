#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int global = 0;

int main(int argc, char** argv) {

    if(argc < 2) {
        fprintf(stderr, "Usage: %s <path to directory>\n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Program name: %s \n", strrchr(argv[0], '/')+1);

    int local = 0;
    pid_t pid = fork();

    if (pid < 0){
        fprintf(stderr, "Invalid PID number : %d", pid);
    }else if(pid == 0){
        printf("Child process:\n");
        global ++;
        local ++;

        printf("Child PID : %d\t parent PID: %d\n", getpid(), getppid());
        printf("Child's global value: %d\t Child's local value: %d\n", global, local);

        execl("/bin/ls", "ls", "-l", argv[1], NULL);
        return EXIT_FAILURE;

    }else {

        int child_status = 0;
        waitpid(pid, &child_status, 0);

        printf("Parent process:\n");
        printf("Parent PID: %d\t Child PID: %d\n", getpid(), pid);
        if (WIFEXITED(child_status)) {
            printf("Child exit code: %d\n", WEXITSTATUS(child_status));
        }
        printf("Parent's global value: %d\t Parent's local value: %d\n", global, local);
        return EXIT_SUCCESS;
    }


}
