//
// Created by Dominik on 13.03.2024.
//

#include <stdlib.h>
#include <stdio.h>
#define BLOCK_SIZE 1024


void reverse_file_by_byte(const char *input_file,const char *output_file) {
    // Otwieramy plik do odczytu i zapisu
    FILE *input_fp = fopen(input_file, "r");
    FILE *output_fp = fopen(output_file, "w");
    if (input_fp == NULL || output_fp == NULL) {
        perror("Files opening error");
        exit(EXIT_FAILURE);
    }
    // Tworzę bufor o rozmiarze jednego znaku do którego będe zapisywał i odczytwał z niego kolejne pojedyncze znaki
    char buffer;
    int i = 0;
    // Za pomocą funkcji fseek ustawiam offset na koniec pliku i następnie zmniejszam go o jeden dopóki funkcja fseek
    // zwraca zero, czyli wykonnuje się poprawnie
    // fread i fwrite przyjmują koljeno :
    // &buffor do zapisu
    // sizeof(char0) rozmiar zapisywancyh danych
    // ilość zapisywanych danych np. 1
    // input_fp/output_fp pilk z którego czytamy albo do któreo zapisujemy
    while (fseek(input_fp, --i, SEEK_END) == 0) {
        fread(&buffer, sizeof(char), 1, input_fp);
        fwrite(&buffer, sizeof(char), 1, output_fp);
    }

    fclose(input_fp);
    fclose(output_fp);
}

void reverse_file_by_block(const char *input_filename, const char *output_filename) {
    FILE *input_fp = fopen(input_filename, "r");
    FILE *output_fp = fopen(output_filename, "w");

    if (input_fp == NULL || output_fp == NULL) {
        perror("Files opening error");
        exit(EXIT_FAILURE);
    }
    // Setting the file pointer at the end of input file
    fseek(input_fp, 0, SEEK_END);
    // Finding the size of the input file
    long int file_size = ftell(input_fp);

    while (file_size > 0) {
        // Calculate the index from which we start read the file, as the length of file minus size of one block
        long int  read_pos = file_size - BLOCK_SIZE;
        // if the last part of text has lower size than block size, we should set the starting index as begin of the file
        if (read_pos < 0) {
            fseek(input_fp, 0, SEEK_SET);
        // Otherwise we start reading bytes from earlier calculated read_pos

    } else {
        fseek(input_fp, read_pos, SEEK_SET);
    }
        //Initializing the buffer of size equals to BLOCK_SIZE and number of bytes we are going to read
        char buffer[BLOCK_SIZE];
        long int bytes_to_read = BLOCK_SIZE;
        if (file_size < BLOCK_SIZE) {
            bytes_to_read = file_size;
        }
        // Saving proper number of bytes from input to buffer
        fread(&buffer, sizeof(char), bytes_to_read, input_fp);

        // Writing the bytes from buffer to output file in reverse order
        for (long int i = bytes_to_read; i > 0; i--) {
            fwrite(&buffer[i - 1], sizeof(char), 1, output_fp);
        }
        // Decrementing the file size of the size of one block
        file_size -= BLOCK_SIZE;
    }

    fclose(input_fp);
    fclose(output_fp);
}

