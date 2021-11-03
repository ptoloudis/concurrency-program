#include <stdio.h>
#include <stdbool.h>
#include "my_pipe.h"



int main(){
    int i, j;
    char c;
    //struct my_pipe *test;

    pipe_init();
    i= pipe_open(5);
    printf("%d\n", i);
    pipe_write(i,'h');
    pipe_write(i,'e');
    pipe_write(i,'l');
    pipe_write(i,'l');
    pipe_write(i,'o');
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    pipe_write(i,'g');
    pipe_write(i,'o');
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);
    j = pipe_read(i, &c);
    printf("%d  %c\n",j,c);

    pipe_free();

    return 0;
}