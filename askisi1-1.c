/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 26 October 2021 19:37:17
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int fd[2],fd2[2];//File descriptor for creating a pipe
volatile int for_exit[2];

/*int size(FILE* fl;) {
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

    if (size != 0) {
        return size;
    }
    return 0;
}*/

void *reader()
{   char    ch[11];
    int result;

    read (fd[0],ch,10);
    printf ("Reader: %s\n", ch);  
    
    while (for_exit[1]!=1){}
    close(fd[0]);
    close(fd[1]);
    result = pipe (fd2);
    if (result < 0){
        perror("pipe ");
        exit(1);
    }
    for_exit[0]=1;
    
    write (fd2[1], ch,10);
    printf ("\nWriter_2: %s\n", ch);
    for (size_t i = 0; i < 1000000; i++){    }
         
    for_exit[0]=2;
}

void *writer()
{   char    str[11],str2[11];

    scanf("%s",str);
    write (fd[1], str,10);
    printf ("Writer: %s\n", str);
    for_exit[1]=1;
    
    while (for_exit[0]!=1){}
    read (fd2[0],str2,10);
    printf ("Reader_2: %s\n", str2); 
    for_exit[1]=2; 
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
   
}