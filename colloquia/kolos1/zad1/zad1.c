////
//// Created by Dominik on 08.04.2024.
////
#include <stdio.h>
#include <dlfcn.h>

int main (int l_param, char * wparam[]){
    int i;
    int tab[20]={1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
    /*
    1) otworz biblioteke
    2) przypisz wskaznikom f1 i f2 adresy funkcji z biblioteki sumuj i srednia
    3) stworz Makefile kompilujacy biblioteke 'bibl1' ladowana dynamicznie
    oraz kompilujacy ten program
    */
    // Tworzymy wskaźnik do otwieranej biblioteki
    void *handler = dlopen("build/libbibl1.so", RTLD_LAZY);
    if (!handler) {
        fprintf(stderr, "Library opening error: %s\n", dlerror());
        return 1;
    }


    // Za pomocą dlsym sprawdzamy czy szukane funkcje są w danej bibliotece i jeśli udało się je znaleźć,
    // to przypisujemy je do odpowiednich wskaźników
    int (*f1)(int *, int);
    double (*f2)(int *, int);
    if(!(f1 = dlsym(handler, "sumuj"))) {
        fprintf(stderr, "Cannot find function sum in library bibl1");
        dlclose(handler);
        return -1;
    }


    if(!(f2 = dlsym(handler, "srednia"))){
        fprintf(stderr, "Cannot find function avg in library bibl1");
        dlclose(handler);
        return -1;

    }
//    if (!f1 || !f2) {
//        fprintf(stderr, "Error: %s\n", dlerror());
//        dlclose(handler);
//        return 1;
//    }


    for (i=0; i<5; i++) printf("Wynik: %d, %lf\n", f1(tab+i, 20-i), f2(tab+i, 20-i));
    dlclose(handler);
    return 0;
}
