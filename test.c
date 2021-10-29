#include <stdio.h>
#include <stdbool.h>
#include "my_pipe.h"

void my_print(int f){
    struct my_pipe *test;
    test = (struct my_pipe *) &f;
    printf("%d\n", test->size);
    printf("%d\n", test->valid);
}

int main(){
    int i;
    i= pipe_open(5);
    //printf("%d\n", test);
    //printf("%p\n", test);
    my_print(i);

    return 0;
}