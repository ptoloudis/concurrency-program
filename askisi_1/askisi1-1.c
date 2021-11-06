/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "my_pipe.h"

volatile int fd; //File Descriptor for Creating a Pipe
int size_pipe;
volatile int for_exit[2];


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
    //j = fseek(fl, 0L, SEEK_SET);
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
    char ch;
    int result, length, i;
    FILE* fl;

    fl = fopen("output_1.txt","w+");
    if (fl == NULL)
    {
        perror("2");
        exit(1);
    }

    do
    {
        i = pipe_read (fd,&ch); //alagi
        if (i != 0)
        {
            fwrite(&ch,1,1,fl);

        //printf("%c",ch);
        }
        for (size_t o = 0; o < 100; o++){}
        
    } while((i != 0));
    
    pipe_close(fd);
    
    result = pipe_open(size_pipe);
    if (result < 0)
    {
        perror("pipe ");
        exit(1);
    }

    fd = result;
    for_exit[0]=1;
    
    length = size(fl);
    printf("2: %d\n",length);

    for ( i = 0; i < length; i++)
    {
        fread(&ch,1,1,fl);
        //printf("%c", ch);
        pipe_write (fd, ch);   //alagi
        for (size_t j = 0; j < 1000000; j++){    } 
    }
    for (size_t i = 0; i < 1000000; i++){    } 
    
    pipe_writeDone(fd);
    fclose(fl);
    
    for_exit[0] = 2;
    
    return 0;
}

void *writer()
{   char ch;
    int length, i, result;
    FILE* fl;

    result = pipe_open(size_pipe);
    printf("%d\n",result);
    if (result < 0)
    {
       perror("pipe ");
       exit(1);
    }

    fd = result;
    for(i = 0; i < 10000; i++){}
    fl = fopen("input.txt","r");
    if (fl == NULL)
    {
        perror("1");
        exit(1);
    }

    length = size(fl);
    printf("1: %d\n", length);

    for ( i = 0; i < length; i++)
    {
        fread(&ch,1,1,fl);
        pipe_write(fd, ch);
        for(int j = 0; j < 100; j++){}
    }

    pipe_writeDone(fd);
    
    fclose(fl);

    

    fl = fopen("output_2.txt","w+");
    if (fl == NULL)
    {
        perror("1");
        exit(1);
    }

    for_exit[1]=1;
    
    while (for_exit[0]!=1){}

    for (size_t o = 0; o < 10000000; o++){}

    int j;
    do
    {
        i = pipe_read(fd,&ch); //alagi
        if (i != 0)
        {
            fwrite(&ch,1,1,fl);
        }
        for (size_t o = 0; o < 10000; o++){}
        j++;
    } while (i != 0);

    for (size_t o = 0; o < 10000000; o++){}

    for_exit[1]=2; 
    
    return 0;
}



/*********************** MAIN ***********************/
int main(int argc, char *argv[]){
    pthread_t tid1,tid2;
    for_exit[0]=0;
    for_exit[1]=0;
    size_pipe = 64;

    pipe_init();
    
    pthread_create(&tid1, NULL, reader, NULL);
    pthread_create(&tid2, NULL, writer, NULL);

    while ((for_exit[1] != 2) && (for_exit[0] != 2)){}

    pipe_free();
    printf("\nbey\n");
    return 0;
}