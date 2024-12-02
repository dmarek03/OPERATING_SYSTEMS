#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
    if (argc == 2){
        char* filename1 = argv[1];
        int fd[2];
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0){
//  zamknij deskryptor do zapisu i wykonaj program sort na filename1
//  w przypadku błędu zwróć 3
            close(fd[1]);
            // Przekierowanie wyjścia programu do wyjścia potoku
            dup2(fd[1], STDOUT_FILENO);
            //Wywołanie sortowanie na pliku filename1
            if(execlp("sort", "sort", filename1, NULL) == -1){
                return 3;
            }


//  koniec
        }else{
            close(fd[0]);
        }
    }
    else if (argc==3){
        char* filename1 = argv[1];
        char* filename2 = argv[2];
        int fd[2];
//  otwórz plik filename2 z prawami dostępu rwxr--r--,
//  jeśli plik istnieje otwórz go i usuń jego zawartość
        // Powinno byc FILE * file_desc = fopen("filename", "r+)
//        if (file_desc != NULL) {
//            fclose(file_desc);
//            // Plik istnieje, więc usuń jego zawartość, otwierając go w trybie "w" (zapis)
//            file_desc = fopen(filename2, "w");
//            if (file_desc == NULL) {
//                perror("fopen");
//                return 1;
//            }
//            fclose(file_desc); // Zamknij plik po wyczyszczeniu zawartości
        int file_desc = fopen(filename2, "rwxr--r--");
        if (file_desc == -1) {
            perror("open");
            return 1;
        }


//  koniec
        pipe(fd);
        pid_t pid = fork();
        if (pid == 0){
//  zamknij deskryptor do zapisu,
//  przekieruj deskryptor standardowego wyjścia na deskryptor pliku filename2 i zamknij plik,
//  wykonaj program sort na filename1
//  w przypadku błędu zwróć 3.
            close(fd[1]);
            dup2(file_desc, STDOUT_FILENO);
            close(file_desc);
            execlp("sort", "sort", filename1, NULL);
            return 3;



//  koniec
        }else{
            close(fd[0]);
        }
    }
    else
        printf("Wrong number of args! \n");

    return 0;
}
