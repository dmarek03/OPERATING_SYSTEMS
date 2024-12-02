//
// Created by Dominik on 08.04.2024.
//
#include <stdlib.h>
#include <stdio.h>
#include "bibl1.h"

/*napisz biblioteke ladowana dynamicznie przez program zawierajaca funkcje:

1) zliczajaca sume n elementow tablicy tab:

int sumuj(int *tab, int n){
 }

2) liczaca srednia n elementow tablicy tab
double srednia(int *tab, int n);

*/
int sumuj(const int *tab, int n){
    if(n < 1){
        fprintf(stderr, "Number of elements must be grater than zero");
    }
    int sum = 0;
    for(int i =0;i<n;i++){
        sum += tab[i];

    }
    return sum;
}

double srednia(int *tab, int n){
    if(n < 1){
        fprintf(stderr, "Number of elements must be greater than zero");
        return -1;
    }
    return (sumuj(tab, n) / (double)n);
}