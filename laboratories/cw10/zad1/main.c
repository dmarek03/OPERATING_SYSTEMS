#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// Tworzymy mutex i condition variables dla mikołaja
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;

// Tworzymy mutexy dla wszystkich 9 reniferów
pthread_mutex_t reindeer_mutex[9] = {
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
        PTHREAD_MUTEX_INITIALIZER,
};

// Utworzenie mutexu i licznika dla powracających reniferów
int back_reindeer_counter = 0;
pthread_mutex_t back_reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;

// Deklarujemy wątki dla mikołaja i reniferów
pthread_t santa_thread ;
pthread_t reindeer_threads[9];




void* reindeer_handler(void *arg){
    int id = *(int*)arg;
    // Ustawiamy tryb anulowania wątku na asynchroniczny aby natychmiast kończył swoje działanie
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pthread_mutex_lock(&reindeer_mutex[id]);
    while (1){
        sleep(rand()%6 + 5);
        //Blokujemy mutex reniferów, które wróciły, aby zwiększyć licznik zliczający liczbę reniferów, kótre wróciły z wakacji
        pthread_mutex_lock(&back_reindeer_mutex);
        printf("Renifer: czeka %d reniferów na Mikołja,%d\n",back_reindeer_counter,id);

        back_reindeer_counter ++;

        // Jeśli wszystkie renifery wróciły to budzimy Mikołaja
        if(back_reindeer_counter == 9){
            printf("Renifer: Budze Mikołaja, %d\n", id);
            pthread_cond_signal(&santa_cond);
            back_reindeer_counter = 0;

        }
        //Odblokowujemy mutex
        pthread_mutex_unlock(&back_reindeer_mutex);

        // Blokujemy mutex dla danego renifera dopóki Mikołaj go nie zwolni
        pthread_mutex_lock(&reindeer_mutex[id]);

        printf("Renifer: Lecę na wakację, %d\n", id);
    }

    return NULL;
}

void* santa_handler(void *args){
    for(int i =0;i<4;i++){
        // Czekamy aż renifery obudzą mikołaja
        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Mikołaj: Budzę się\n");

        printf("Mikołaj: Dostarczam zabawki\n");
        sleep(rand()%+3+2);

        for(int j= 0; j< 9 ; j++){
            pthread_mutex_unlock(&reindeer_mutex[j]);
        }
        printf("Mikołaj:Zasypiam\n");
    }

    for (int j = 0; j < 9; ++j) {
        pthread_cancel(reindeer_threads[j]);
    }

    return NULL;

}

int main(void) {
    int ids[9];
    //Tworzymy wątki dla mikołaja i reniferów
    pthread_create(&santa_thread, NULL, santa_handler, NULL);
    for (int i = 0; i < 9; ++i) {
        ids[i] = i;
        pthread_create(&reindeer_threads[i], NULL, reindeer_handler, &ids[i]);
    }
    // Czekamy na wykonanie się wątków Mikołaja i reniferów
    pthread_join(santa_thread, NULL);
    for (int i = 0; i < 9; ++i) {
        pthread_join(reindeer_threads[i], NULL);
    }
    return 0;
}
