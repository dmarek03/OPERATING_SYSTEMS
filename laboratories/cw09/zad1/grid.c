#include "grid.h"
#include <pthread.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>


const int grid_width = 30;
const int grid_height = 30;
const int number_of_cells = grid_width * grid_height;
typedef struct ThreadData{
    char *source;
    char *destination;
    int start;
    int end;
}ThreadData;

pthread_t *threads = NULL;
ThreadData *threads_data = NULL;

void free_memory(){
    free(threads);
    free(threads_data);
}

void ignore_handler(int signum){}

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}


void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst)
{
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

void *update_cell(void *args){
    ThreadData *thread_args = (ThreadData*)args;
    int start = thread_args->start;
    int end = thread_args->end;

    while (true){
        for(int k = start; k < end ;k ++){
            thread_args->destination[k] = is_alive(k/grid_width, k%grid_height, thread_args->source);

        }
        pause();
        char* tmp = thread_args->source;
        thread_args->source= thread_args->destination;
        thread_args->destination = tmp;
    }


}


void update_grid_concurrent(char* src, char *dts, int threads_number){
    if(threads==NULL){
        static struct sigaction action;
        sigemptyset(&action.sa_mask);
        action.sa_handler = ignore_handler;
        sigaction(SIGUSR1, &action, NULL);
    }


    threads = malloc(threads_number* sizeof(pthread_t));
    threads_data = malloc(threads_number* sizeof(ThreadData));

    int r = number_of_cells % threads_number, block_size;
    for(int i = 0 ;i < threads_number ;i ++){
        block_size = number_of_cells / threads_number + (r-- > 0 ? 1 : 0);
        threads_data[i].source = src;
        threads_data[i].destination = dts;
        threads_data[i].start = i > 0 ? threads_data[i-1].end+1:0;
        threads_data[i].end = threads_data[i-1].start + block_size-1;

        pthread_create(&threads[i], NULL, update_cell, (void*)&threads_data[i]);
    }

    for(int i=0;i<threads_number;i++){
        pthread_kill(threads[i], SIGUSR1);
    }

}