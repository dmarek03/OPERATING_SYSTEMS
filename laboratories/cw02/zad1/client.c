#include <stdio.h>
#define MAX_ITER 1000

#ifndef DYNAMIC_DLOPEN
#include "collatz.h"
#endif //NOT DYNAMIC_DLOPEN

#ifdef DYNAMIC_DLOPEN
#include <dlfcn.h>
#endif //DYNAMIC_DLOPEN

int main(void) {
#ifdef DYNAMIC_DLOPEN

    printf("DYNAMIC LOADING VARIANT \n");
    // Tworzę hanlder do otwarcia biloteki
    void *handler = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handler) {
        printf("Opening library error \n");
        return 1;
    }
    // Tworzę wskaźnik do funkcji, którą chcę załączyć z biblioteki (podaje typy argumentów jakie przyjmuje)
    int (*test_collatz_convergence)(int input, int max_iter);
    // Za pomca dlsym szukamy funkcji o podanej nazwie w biblotece
    test_collatz_convergence = dlsym(handler, "test_collatz_convergence");
    if (dlerror() != 0) {
        printf("Function error \n");
        dlclose(handler);
        return 1;
    }
#endif
    int num;

    for(int i=0;i<5;i++){
        printf("Enter number:\n");
        scanf("%d",&num);
        int iterations = test_collatz_convergence(num,MAX_ITER);
        if(iterations > -1) {
            printf("Number %d converges to 1 in %d iterations\n",num, iterations);
        }
        else{
            printf("Max number of iterations was reached\n");
        }
    }

#ifdef DYNAMIC_DLOPEN
    dlclose(handler);
#endif
    return 0;
}

