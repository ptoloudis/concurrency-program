#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "header.h"

int flag, buf_size;
char *buf;
co_t co1, co2, co_main;
FILE* input_file;


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

void *consumer()
{   
    FILE* fl;

    printf("Start consumer\n");
    fl = fopen("output.txt","w+");
    if (fl == NULL)
    {
        perror("2");
        exit(1);
    }

    while (flag == 0)
    {
        fwrite(buf,1, buf_size,fl);
        mycoroutines_switchto(&co1);
    }
    
    fclose(fl);
    printf("End consumer\n");
    mycoroutines_switchto(&co_main);

    return 0;
}


void *producer()
{   
    int length, i;

    printf("Start producer\n");
    length = size(input_file );
    printf("The length is: %d\n", length);

    for ( i = 0; i < length; i= i + 256)
    {
        if (length - i < 256)
        {
            buf_size = length - i;
            buf = realloc(buf,buf_size);
            fread(buf,1, buf_size,input_file );
        }
        else
        {
            fread(buf,1, buf_size,input_file );
        }
        
        mycoroutines_switchto(&co2);
    }
    
    fclose(input_file );

    flag = 1;
    printf("End producer\n");
    mycoroutines_switchto(&co2);

    return 0;
}



/*********************** MAIN ***********************/
int main(int argc, char *argv[]){
    
    flag = 0;
    buf_size = 256;
    buf = (char*)malloc(256);
    input_file = fopen(argv[1],"r");
    if (input_file == NULL)
    {
        perror("1");
        exit(1);
    }

    mycoroutines_init(&co2);
    mycoroutines_init(&co1);
    mycoroutines_init(&co_main);
    
    mycoroutines_create(&co1, (void*)&producer, 0);
    mycoroutines_create(&co2, (void*)&consumer, 0);

    
    mycoroutines_switchto(&co1);

    mycoroutines_destroy(&co1);
    mycoroutines_destroy(&co2);
    mycoroutines_destroy(&co_main);
    
    free(buf);

    printf("complited\n");
    return 0;
}
