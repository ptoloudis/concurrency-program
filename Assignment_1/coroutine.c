#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>
#include "header.h"

int flag;
char buf[256];
co_t co1, co2, co_main;

//co_t *curr = NULL, *nxt = NULL;

int size(FILE* fl) 
{
    int j,size;

    j = fseek(fl, 0L, SEEK_END);
    if (j == -1) 
    {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    size = ftell(fl);
    if (size == -1) 
    {
        ferror(fl);
        perror("no ftell argument file\n");
        fclose(fl);
        return -1;
    }
    rewind(fl);
    
    if (j == -1) 
    {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    if (size != 0) 
    {
        return size;
    }
    return 0;
}

void *reader()
{   
    FILE* fl;

    fl = fopen("output.txt","w+");
    if (fl == NULL)
    {
        perror("2");
        exit(1);
    }

    while (flag == 0)
    {
        fwrite(buf,1, 256,fl);
        mycoroutines_switchto(&co1);
    }
    
    fclose(fl);
    mycoroutines_switchto(&co_main);

    return 0;
}


void *writer()
{   
    int length, i;
    FILE* fl;

    fl = fopen("medium.txt","r");
    if (fl == NULL)
    {
        perror("1");
        exit(1);
    }

    length = size(fl);
    printf("1: %d\n", length);

    for ( i = 0; i < length; i++)
    {
        fread(buf,1,256,fl);
        mycoroutines_switchto(&co2);
    }
    
    fclose(fl);

    flag = 1;
    mycoroutines_switchto(&co2);

    return 0;
}



/*********************** MAIN ***********************/
int main(int argc, char *argv[]){
    

    mycoroutines_init(&co_main);
    mycoroutines_init(&co1);
    mycoroutines_init(&co2);
    
    mycoroutines_create(&co1, (void*)&reader , NULL);
    mycoroutines_create(&co2, (void *)&writer, NULL);
    //mycoroutines_create(&main, writer, NULL);

    mycoroutines_switchto(&co1);

    mycoroutines_destroy(&co1);
    mycoroutines_destroy(&co2);

    printf("\nbey\n");
    return 0;
}
