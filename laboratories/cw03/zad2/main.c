#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#define MAX_BUFFER_SIZE 1024

void browse_current_directory(const char *dir_path){
    long long int total_file_size = 0;
    // Tworzmy strukturę typu DIR reprezentującą strumień katalogowy  do której przypisujemy wskaźnik do katalogu podanego jak dir_path
    DIR *dir =  opendir(dir_path);
    // Tworzymy strukturę dirent , która składa się:
    // * ino_t d_ino – numer i-węzła pliku
    // * char d_name[] – nazwa pliku
    struct dirent *current_file;
    struct stat f_stat;
    char path_buffer[MAX_BUFFER_SIZE];
    if(dir == NULL){
        printf("Directory opening error");
    }
    // Odczytujemy kolejne pliki z katalogu
    while((current_file = readdir(dir)) != NULL){
        size_t file_name_length = strlen(current_file->d_name);

        if(file_name_length + strlen(dir_path) >= MAX_BUFFER_SIZE){
            printf("Max file name length was exceeded \n");
        }

        // Tworzymy pełną scieżkę do naszego pliku
        strcpy(path_buffer, dir_path);
        strcat(path_buffer, "/");
        strcat(path_buffer, current_file->d_name);
        // W strukturze f_stat zapisujemy dane o naszym pliku
        stat(path_buffer, &f_stat);


        // Sprawdzamy czy plik nie jest katalogiem i wówczas wypisujemy jego nazwę i rozmiar
        if (!S_ISDIR(f_stat.st_mode)) {
            printf("%s [%ld B]\n", current_file->d_name, f_stat.st_size);
            total_file_size += f_stat.st_size;
        }
    }

    printf("Total size of files: %lld B\n", total_file_size);
    closedir(dir);


}



int main() {
    const char *path = ".";
    browse_current_directory(path);

}
