/*
Team : 10
Names : Apostolopoulou Ioanna & Toloudis Panagiotis
AEM : 03121 & 02995
*/

#include <stdio.h>
#include <stdlib.h>
#include "my_pipe.h"

struct my_pipe *arr;
int many_pipe;

struct my_pipe{
    char *str;
    int size;
    int valid; //  0 is Not Valid, 1 is Valid
    int write; // 0 to Not Write, 1 to Write, 2 to Close 
    int output;
    int input;
    int full; 
};

void pipe_init()
{
    arr = NULL;
    many_pipe = 0;
}

void pipe_free()
{
    for (size_t i = 0; i < many_pipe; i++)
    {
        if (arr[i].str != NULL)
        {
            free(arr[i].str);
        }    
    }

    free(arr);    
}

int pipe_open(int size)
{
    int i,j;

    i = many_pipe;

    for (j = 0; j < i; j++)
    {
        if (arr[j].valid == 0)
        {
            break;
        }
    }
    if ((i == 0) || (arr[j].valid != 0))
    {
        many_pipe++;
        arr = (struct my_pipe *) realloc (arr,many_pipe * sizeof(struct my_pipe));
    }

    i = j;

    
    if (arr == NULL)
    {
        perror("malloc struct\n");
        exit(1);
    }

    arr[i].str = (char *) calloc (size , sizeof(char));

    if (arr[i].str == NULL)
    {
        perror("malloc array\n");
        exit(1);
    }

    arr[i].size =  size;
    arr[i].valid = 1;
    arr[i].output= 0;
    arr[i].input = 0;
    arr[i].write = 0;
    arr[i].full = 0;

    return i;
}

int pipe_writeDone(int p)
{
    if (arr[p].write == 2)
    {
        return -1;
    }

    arr[p].write = 2;

    return 1;
}

void pipe_close(int p)
{
    free(arr[p].str);
    arr[p].str = NULL;
    arr[p].valid =0;
}

int pipe_write(int p, char c)
{
    int input, size;

    if (arr[p].write == 2)
    {
        return -1;
    }
    
    arr[p].write = 1;
    input = arr[p].input;
    
    size = arr[p].size;
    
    while ( arr[p].full == 1){} 
    
    if (input == size)
    {
        arr[p].input = 0;
        input = 0;
    }
    
    arr[p].str[input] = c;
    arr[p].input++;
    arr[p].write = 0;

    if (arr[p].input == arr[p].output)
    {
        arr[p].full = 1;
    }
        
    return 1;
}

int pipe_read(int p, char *c)
{
    int out;

    if (arr[p].valid == 0)
    {
        return -1;
    }
        
    
    if ((arr[p].input == arr[p].output) && (arr[p].full == 0))
    {
        if (arr[p].write == 2)
        {
            return 0;
        }
            
    }
    
    while ((arr[p].input == arr[p].output) && (arr[p].full == 0)){} 
    
    if (arr[p].output == arr[p].size)
    {
        arr[p].output = 0;
    }
    
    out = arr[p].output;
    *c = arr[p].str[out];

    if(c == NULL)
    {
        perror("5");
    }

    arr[p].output++;

    if (arr[p].full)
    {
        arr[p].full = 0;
    }
        
    return 1;   
}