#include <stdio.h>
struct my_pipe{
    //int min;
    //int max;
    int size;
    
};
void my_print(int *f){
    struct my_pipe *test;
    test = (struct my_pipe *) f;
    printf("%d\n", test->size);
}

int main(){
    struct my_pipe test;
    int *i;
    test.size =645;
    i = (int *) &test;
    //printf("%d\n", test);
    //printf("%p\n", test);
    my_print(i);

    return 0;
}