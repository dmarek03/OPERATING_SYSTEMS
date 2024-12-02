#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

struct pak{
 int i;
 char lit;
};

int main (int lpar, char *tab[]){
    int w1;
    struct pak ob1;
    // 1) utworzyc potok nazwany 'potok1'
    //
    char *myfifo = "/tmp/potok1";
    mkfifo(myfifo, 0666);
    if((w1 = open(myfifo, O_RDONLY)) == -1){
        printf("Opening pipe error");
        exit(EXIT_FAILURE);
    }
    while (1){
    // 2) wyswietlic obiekt otrzymany z potoku
    //
    //
        if(read(w1, &ob1, sizeof(ob1))==-1){
            perror("Reading from pipe error");
            exit(EXIT_FAILURE);
        }

            printf("otrzymano: %d %c\n", ob1.i, ob1.lit);
            fflush(stdout);


    }
    close(w1);
    return 0;
}
