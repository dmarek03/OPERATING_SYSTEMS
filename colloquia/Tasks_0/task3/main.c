#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/types.h>
#include <pthread.h>

void* hello(void* arg){
        
        sleep(1);
        while(1){
                printf("Hello world from thread number %d\n", *(int*)arg);
		/****************************************************
			przerwij dzialanie watku jesli bylo takie zadanie
		*****************************************************/                
                printf("Hello again world from thread number %d\n", *(int*)arg);
                sleep(2);
        }
        return NULL;
}


int main(int argc, char** args){

       if(argc !=3){
	    printf("Not a suitable number of program parameters\n");
    	return(1);
   	}

       int n = atoi(args[1]);
       int i;
       pthread_t threads[n];

	/**************************************************
	    Utworz n watkow realizujacych funkcje hello
	**************************************************/

        for(i = 0; i < n ;i ++){
            int* thread_num = malloc(sizeof(int)); // Przypisz numer wątku do wskaźnika
            *thread_num = i + 1; // Numeracja wątków od 1
            pthread_create(&threads[i], NULL, hello, (void*)thread_num);
        }
        

        i = 0;
        while(i++ < atoi(args[2])) {
                printf("Hello from main %d\n",i);
                sleep(2);
        }
        
        i = 0;


	/*******************************************
	    "Skasuj" wszystke uruchomione watki i poczekaj na ich zakonczenie
	*******************************************/
        for(i = 0; i < n; i++) {
            if(pthread_cancel(threads[i]) != 0) {
                perror("pthread_cancel");
                exit(EXIT_FAILURE);
            }
        }

        for(i = 0; i < n; i++) {
            if(pthread_join(threads[i], NULL) != 0) {
                perror("pthread_join");
                exit(EXIT_FAILURE);
            }
        }
        printf("DONE");
        
        
        return 0;
}

