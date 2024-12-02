//
// Created by Dominik on 08.05.2024.
//

#ifndef ZAD1_GRID_H
#define ZAD1_GRID_H
#pragma once
#include <stdbool.h>
extern const int grid_width;
extern const int grid_height;
extern const int number_of_cells;
char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void update_grid(char *src, char *dst);
void free_memory();
void update_grid_concurrent(char *src, char *dst, int threads_no);
#endif //ZAD1_GRID_H
