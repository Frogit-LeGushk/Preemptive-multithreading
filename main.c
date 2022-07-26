#include "threads.h"
#include "timers.h"
#include <stdio.h>

int thread_entry_1(void * arg);
int thread_entry_2(void * arg);
int thread_entry_3(void * arg);


int main(void)
{
    int n;
    int * ret;
    int arr_args[3] = {42, 84, 168};
    int arr_id  [3];

    /** 1) add threads in pool before run scheduler **/
    arr_id[0] = push_thread(&thread_entry_1, &arr_args[0]);
    arr_id[1] = push_thread(&thread_entry_2, &arr_args[1]);
    arr_id[2] = push_thread(&thread_entry_3, &arr_args[2]);

    /** 2) run execution of thread pool **/
    ret = run_scheduler(&n);

    /** 3) check status of terminated (return number) **/
    for(int i = 0; i < n; i++) printf("Return address thread[%d]: %d\n", i + 1, ret[arr_id[i]]);
    return 0;
}

int thread_entry_1(void * arg)
{
    int i = 5;
    while(i-- >= 0)
    {
        printf("I am thread 1, got argument: %d\n", *(int *)arg);
        delay(1);
    }
    return 5;
}
int thread_entry_2(void * arg)
{
    int i = 10;
    while(i-- >= 0)
    {
        printf("I am thread 2, got argument: %d\n", *(int *)arg);
        delay(1);
    }
    return 10;
}
int thread_entry_3(void * arg)
{
    int i = 15;
    while(i-- >= 0)
    {
        printf("I am thread 3, got argument: %d\n", *(int *)arg);
        delay(1);
    }
    return 15;
}
