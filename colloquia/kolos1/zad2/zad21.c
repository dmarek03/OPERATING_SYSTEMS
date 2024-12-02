#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>


int fd[2];
int flag = 1;
int number = 0;


/*program tworzy dwa procesy potomne. Proces macierzysty co sekunde wysyła
SIGUSR1 do procesu potomnego 1. Proces potomny 1 po otrzymaniu sygnału przesyła
kolejną liczbe przez potok nienazwany do procesu potomnego 2, który wyswietla
te liczbe */

//
//

void sigusr1_handler(){
    int size = sizeof(int);
    write(fd[1], &size, sizeof(int));
    write(fd[1], &number, sizeof(int));
    number++;

}

void sigint_handler(){
    if (flag == 0){
        flag = 1;
    } else{
        flag = 0;
    }
}
int main (int lpar, char *tab[]){
    pid_t pid1, pid2;
    //
    //
    pipe(fd);
    pid1 = fork();
    int i, d;
    if (pid1<0){
        perror("fork");
        return 1;
    }
    else if (pid1==0){//proces 1
        close(fd[0]);


        while(1)
        {   if(flag == 1) {
                signal(SIGUSR1, sigusr1_handler);
            }
        }
    }

    else{
        pid2 = fork();

        if(pid2<0){
            perror("fork");
            return 1;
        }

        else if (pid2==0)
        {
            close(fd[1]);
            while(1){
                if (flag==1) {
                    read(fd[0], &d, sizeof(int));
                    read(fd[0], &i, sizeof(int));
                    printf("przyjeto %d bajtow, wartosc:%d\n", d, i);
                }
            }

        }

        else {
            close(fd[0]);
            close(fd[1]);
            signal(SIGINT, sigint_handler);

            while(1) {

                signal(SIGINT, sigint_handler);
                if (flag == 1) {
                    kill(pid1, SIGUSR1);
                    printf("wyslano SIGUSR1\n");
                } else {
                    printf("wylaczono wysylanie sygnalow\n");
                }

                sleep(1);
            }
        }

    }

}