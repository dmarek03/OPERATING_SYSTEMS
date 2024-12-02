#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "[Error] Running the program without an argument\n");
        return -1;
    }

    int threads_number = atoi(argv[1]);
    if((threads_number < 0 ) | (threads_number > number_of_cells)){
        printf("Incorrect number of threads: %d\n",threads_number);
        printf("Please enter number from range 1 to %d\n", number_of_cells);
        return -1;
    }

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode.

    char *foreground = create_grid();
    char *background = create_grid();
    char *tmp;

    init_grid(foreground);

    while (true)
    {
        draw_grid(foreground);
        usleep(500 * 1000);

        // Step of the simulation.
        update_grid_concurrent(foreground, background, threads_number);
        tmp = foreground;
        foreground = background;
        background = tmp;
    }

    endwin(); // End curses mode.
    destroy_grid(foreground);
    destroy_grid(background);
    free_memory();

    return 0;
}