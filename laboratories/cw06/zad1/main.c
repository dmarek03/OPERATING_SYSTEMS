#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define FUNC(x) (4.0 / ((x)*(x) + 1))

void measure_time(long double (*fun)(long double width, int num_processes), long double width, int num_processes) {
    struct timespec start_ts, end_ts;
    long double elapsed_time;
    long double result;

    clock_gettime(CLOCK_REALTIME, &start_ts);
    result = fun(width, num_processes);
    clock_gettime(CLOCK_REALTIME, &end_ts);

    elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) / 1e9L;
    printf("\nWidth: %.10Lf\tNum Processes: %d\tResult: %.10Lf\tTime taken: %.10Lf seconds\n\n", width, num_processes, result, elapsed_time);

    FILE *fp;
    fp = fopen("report.txt", "a");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return;
    }
    fprintf(fp, "Width: %.10Lf\nNum Processes: %d\nResult: %.10Lf\nTime taken: %.10Lf seconds\n\n", width, num_processes, result, elapsed_time);
    fclose(fp);
}

long double calculate_integral(long double width, int num_processes) {
    int i;
    long double sum = 0.0;
    pid_t pid;
    // Tworzymy tablicę o num_processes wiersza do przechowywania potoków
    int pipes_fd[num_processes][2];
    long double result;

    long double interval = 1.0 / num_processes;

    for (i = 0; i < num_processes; i++) {
        // Tworzymy nowy potok nie nazwany
        pipe(pipes_fd[i]);
        // Tworzymy proces potomny
        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(EXIT_FAILURE);
         // Proces potomny
        } else if (pid == 0) {
            // Zamykamy nie używany dekskryptor potoku, w tym przypadku końcówkę służącą do odczytu
            close(pipes_fd[i][0]);

            long double x = (double)i / num_processes;
            long double local_sum = 0.0;
            long double x_start = x;
            long double x_end = x + interval;

            while (x_start < x_end) {
                local_sum += FUNC(x_start) * width;
                x_start += width;
            }
            // Przesyłamy dane przez potok podająć następujące argumenty:
            // 1) pipes_fd[i][1] -> końcówka i-tego potoku odpowiedzialana za zapis danych
            // 2) &local_sum -> buffor do, którego zapsujemy dane
            // 3) sizeof(local_sum)) -> rozmiar zapoisywanych danych
            write(pipes_fd[i][1], &local_sum, sizeof(local_sum)); // write local sum to pipe
            exit(EXIT_SUCCESS);
        } else { // Proces macierzysty
            // Zamykamy końcówkę potoku odpowiedzialną za zapis
            close(pipes_fd[i][1]);

        }
    }
    for(int j = 0; j < num_processes; j++){
        // Odczytujemy dane z potoku podająć następujące argumenty:
        // 1) pipes_fd[j][0] -> końcówka i-tego potoku odpowiedzialana za odczyt danych
        // 2) &result -> buffor do, którego zapsujemy odczytane dane
        // 3) sizeof(result)) -> rozmiar zapisywanych danych
        read(pipes_fd[j][0], &result, sizeof(result));
        sum += result;
    }

    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <width> <num_processes>\n", argv[0]);
        return EXIT_FAILURE;
    }

    long double width = strtod(argv[1], NULL);
    int num_processes = atoi(argv[2]);

    measure_time(calculate_integral, width, num_processes);

    return EXIT_SUCCESS;
}