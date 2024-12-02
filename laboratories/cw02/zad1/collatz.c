//
// Created by Dominik on 06.03.2024.
//


int collatz_conjecture(int input){
    if(input % 2 == 0) {
        return input /2;
    }
    return 3 * input + 1;

}

int test_collatz_convergence(int input, int max_iter){
    int cnt = 0;
    while(cnt < max_iter && input != 1){
        input = collatz_conjecture(input);
        cnt ++;

    }
    if(input == 1 && cnt < max_iter) {
        return cnt;
    }
    return -1;
}
