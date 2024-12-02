#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "pipe_specs.h"

int main() {
    double lower_limit, upper_limit, width;
    double result;
    int input_pipe_fd ;
    int output_pipe_fd;

    printf("Enter the integration interval [a, b]: ");
    scanf("%lf %lf", &lower_limit, &upper_limit);
    printf("Enter the width of the interval: ");
    scanf("%lf", &width);

    //Tworzymy potok o nazwie in_pipe_name w bieżącym katalogu
    // O_WRONLY - > otwieramy potok tylko  w trybie do zapisu
    if ((input_pipe_fd = open(in_pipe_name, O_WRONLY)) == -1) {
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }

    // Zapisujemy dane do potoku
    if (write(input_pipe_fd, &lower_limit, sizeof(double)) == -1 ||
        write(input_pipe_fd, &upper_limit, sizeof(double)) == -1 ||
        write(input_pipe_fd, &width, sizeof(double)) == -1) {
        perror("Error sending data through the pipe");
        close(input_pipe_fd);
        exit(EXIT_FAILURE);
    }


    close(input_pipe_fd);

    // Tworzymy potok out_pipe_name
    // O_WDONLY -> otwierany potok służy tylko do zapisu danych
    if ((output_pipe_fd = open(out_pipe_name, O_RDONLY)) == -1) {
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }

    // Odczytujemy dane z potoku i zapisujemy je do zmiennej result
    if (read(output_pipe_fd, &result, sizeof(double)) == -1) {
        perror("Error reading data from the pipe");
        close(output_pipe_fd);
        exit(EXIT_FAILURE);
    }

    close(output_pipe_fd);

    printf("Result of integration: %.6f\n", result);

    return 0;
}
