#include "reverse.h"
#include <stdio.h>
#include <time.h>

long double measure_time(void (*fun)(const char*, const char*), const char* input_f, const char* output_f) {
    struct timespec start_ts, end_ts;
    long double elapsed_time;
    clock_gettime(CLOCK_REALTIME, &start_ts);
    fun(input_f, output_f);
    clock_gettime(CLOCK_REALTIME, &end_ts);

    elapsed_time = (end_ts.tv_sec - start_ts.tv_sec) + (end_ts.tv_nsec - start_ts.tv_nsec) / 1000000000.0L;

    return elapsed_time;
}


int main(int argc, char **argv) {

    const char *input_filename = argv[1];
    const char *output_filename = argv[2];

    // Otwieramy plik do zapisu (flaga "w"), jeśli ten plik wcześniej istniał to to zostanie skasowany a na
    // jego miejsce powstanie nowy
    FILE *fp = fopen("pomiar_zad_2.txt", "w");

    if(fp == NULL){
        printf("File opening error");
    }

    long double elapsed_time_1;
    long double elapsed_time_2;

    elapsed_time_1 = measure_time(reverse_file_by_byte, input_filename, output_filename);
    elapsed_time_2 = measure_time(reverse_file_by_block, input_filename, output_filename);

    fprintf(fp, "Time elapsed for 1 byte approach: %Lf\n", elapsed_time_1);
    fprintf(fp, "Time elapsed for 1024 bytes approach : %Lf", elapsed_time_2);

    fclose(fp);
    return 0;
}
