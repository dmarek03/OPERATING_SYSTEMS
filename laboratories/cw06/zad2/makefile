CC = gcc
FLAGS = -Wall
DIR_NAME = build


program1: program1.c
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) program1.c -o $(DIR_NAME)/program1

program2: program2.c
	mkdir -p $(DIR_NAME)
	$(CC) $(FLAGS) program2.c -o $(DIR_NAME)/program2

run_program1: program1
	./$(DIR_NAME)/program1

run_program2: program2
	./$(DIR_NAME)/program2

create_pipe:
	mkfifo /tmp/int_calc_in.fifo
	mkfifo /tmp/int_calc_out.fifo

run_programs: clean create_pipe run_program1 run_program2

clean:
	rm -rf $(DIR_NAME) /tmp/int_calc_in.fifo  /tmp/int_calc_out.fifo
