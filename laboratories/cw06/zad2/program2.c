#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "pipe_specs.h"

double func(double x) {
    return 4.0 / (x*x + 1);
}

int main() {
    double lower_limit, upper_limit, width;
    double result = 0.0;
    int input_pipe_fd;
    int output_pipe_fd;

    // Tworzymy potok o nazwie in_pipe_name, wykorzystywany do odczytu danych
    if ((input_pipe_fd = open(in_pipe_name, O_RDONLY)) == -1) {
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }

    // Odczytujemy dane z potoku i zapisujemy je do zmiennej lower_limit
    if (read(input_pipe_fd, &lower_limit, sizeof(double)) == -1 ||
        read(input_pipe_fd, &upper_limit, sizeof(double)) == -1 ||
        read(input_pipe_fd, &width, sizeof(double)) == -1) {
        perror("Error reading data from the pipe");
        close(input_pipe_fd);
        exit(EXIT_FAILURE);
    }

    close(input_pipe_fd);


    for (double x = lower_limit; x < upper_limit; x += width) {
        result += func(x) * width;
    }

    // Tworzymy potok o nazwie out_pipe_name używany tylko do zapisu
    if ((output_pipe_fd = open(out_pipe_name, O_WRONLY)) == -1) {
        perror("Error opening the named pipe");
        exit(EXIT_FAILURE);
    }

    // Send the result through the pipe
    if (write(output_pipe_fd, &result, sizeof(double)) == -1) {
        perror("Error sending data through the pipe");
        close(output_pipe_fd);
        exit(EXIT_FAILURE);
    }

    // Close the pipe
    //close(output_pipe_fd);

    return 0;
}
