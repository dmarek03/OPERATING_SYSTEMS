#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

struct pak{
 int i;
 char lit;
} ob1;
int input_pipe;
/*program przy kazdym uzyciu klawiszy ctrl-c (SIGINT) ma wyslac obiekt o1 przez
potok nazwany 'potok1'*/
//
//

void handler(int signum){
    if(write(input_pipe, &ob1, sizeof(ob1))==-1){
        perror("Error sending data through pipe");
        exit(EXIT_FAILURE);
    }

}

int main (int lpar, char *tab[]){

    //
    //
    char *potok1 = "/tmp/potok1";

    ob1.i=0;
    ob1.lit='a';
    if((input_pipe= open(potok1, O_WRONLY))==-1){
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, handler);
    while(1) {
        printf("%d %c\n",ob1.i,ob1.lit); fflush(stdout);
        ob1.i++;
        ob1.lit=ob1.lit<'e'?ob1.lit+1:'a';
        sleep(1);
  }
    close(input_pipe);
    return 0;
}
