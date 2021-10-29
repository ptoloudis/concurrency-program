/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 28 October 2021 14:18:40
*/

#include <stdio.h>
#include <stdlib.h>
#include "my_pipe.h"

/*struct my_pipe{
    char *arr;
    int size;
    int valid; // 0 is valid
    int min;
    int max; 
};*/


int pipe_open(int size){
    struct my_pipe new;
    int *i,k;

    
    /*new = (struct my_pipe *) malloc (sizeof(struct my_pipe));
    if (new == NULL){
        perror("malloc struct\n");
        exit(1);
    }*/
    new.arr = (char *) malloc (size *sizeof(char));
    if (new.arr == NULL){
        perror("malloc array\n");
        exit(1);
    }
    new.size =  size;
    new.valid = 0;
    new.min = 0;
    new.max = 0;
    i = (int *) &new;
    k = i;
    return ( k );
}