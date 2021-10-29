/*
Name: Panagiotis Toloudis
Github: ptoloudis
Modified on: 28 October 2021 14:19:22
*/

struct my_pipe{
    char *arr;
    int size;
    int valid; // 0 is valid
    int write;
    int min;
    int max; 
};

int pipe_open(int size);
int pipe_write(int p, char c);
int pipe_writeDone(int p);
int pipe_read(int p, char *c);