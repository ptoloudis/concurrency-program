/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 26 October 2021 19:37:17
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int fd[2];//File descriptor for creating a pipe
volatile int for_exit[2];


int size(FILE* fl) {
    int j,size;
    
    j = fseek(fl, 0L, SEEK_END);
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    size = ftell(fl);
    if (size == -1) {
        ferror(fl);
        perror("no ftell argument file\n");
        fclose(fl);
        return -1;
    }

     j = fseek(fl, 0L, SEEK_SET);
    if (j == -1) {
        ferror(fl);
        perror("no fseek the argument file\n");
        fclose(fl);
        return -1;
    }

    if (size != 0) {
        return size;
    }
    return 0;
}

void *reader()
{   char    ch;
    int result, megetos,i;
    FILE* fl;

    fl = fopen("output_1.txt","w+");
    if (fl == NULL)
    {
        perror("2");
        exit(1);
    }

    read (fd[0], &megetos,sizeof(int));
    for ( i = 0; i < megetos; i++){
        read (fd[0],&ch,1); //alagi
        fwrite(&ch,1,1,fl);
    } 
    while (for_exit[1]!=1){} 
    close(fd[0]);
    close(fd[1]);
    result = pipe (fd);
    if (result < 0){
        perror("pipe ");
        exit(1);
    }
    for_exit[0]=1;
    
    
    megetos = size(fl);
    printf("2: %d\n",megetos);
    write (fd[1], &megetos,sizeof(int));
    for ( i = 0; i < megetos; i++){
        fread(&ch,1,1,fl);
        write (fd[1], &ch,1);   //alagi
    }
    for (size_t i = 0; i < 1000000; i++){    } 

    fclose(fl); 
    for_exit[0]=2;
    return 0;
}

void *writer()
{   char    ch;
    int megetos,i;
    FILE* fl;
    
    fl = fopen("input.txt","r");
    if (fl == NULL)
    {
        perror("1");
        exit(1);
    }

    megetos = size(fl);
    printf("1: %d\n",megetos);
    write (fd[1], &megetos,sizeof(int));
    for ( i = 0; i < megetos; i++){
        fread(&ch,1,1,fl);
        write (fd[1], &ch,1);   //alagi
    }
    
    fclose(fl);
    fl = fopen("output_2.txt","w+");
    if (fl == NULL)
    {
        perror("1");
        exit(1);
    }
    for_exit[1]=1;
    
    while (for_exit[0]!=1){}
    read (fd[0], &megetos,sizeof(int));
    for ( i = 0; i < megetos; i++){
        read (fd[0],&ch,1); //alagi
        fwrite(&ch,1,1,fl);
    } 
    for_exit[1]=2; 
    return 0;
}

int main()
{
   pthread_t tid1,tid2;
   int result;
   for_exit[0]=0;
   for_exit[1]=0;

   result = pipe (fd);
   if (result < 0){
       perror("pipe ");
       exit(1);
   }

   pthread_create(&tid1,NULL,reader,NULL);
   pthread_create(&tid2,NULL,writer,NULL);

   while (for_exit[1]!=2 && for_exit[0]!=2){   }
   printf("bey\n");
   return 0;
   
}