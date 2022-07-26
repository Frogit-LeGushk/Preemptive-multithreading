#include "threads.h"
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/time.h>

/** saved registers of thread **/
struct Context {
    uint64_t rflafs;
    uint64_t r8;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbp;
    uint64_t rbx;
	uint64_t rip;
} __attribute__((packed));


/** environment of scheduler **/
#define                 __MAX_THREADS 1024 * 1024 * 4 // 4mb
static const uint32_t   __STACK_SIZE = 1024 * 1024;
static struct Context * __POOL_THREADS[__MAX_THREADS];
static bool             __TERMINATED[__MAX_THREADS];
static int              __RET_ADDRESS[__MAX_THREADS];
static void *           __FREE_PTRS[__MAX_THREADS];
static uint32_t         __TOP = 0;
static uint32_t         __EXECUTED = 0;
static jmp_buf          __JMP_BUFFER;


/** period of switching, 50ms **/
static struct itimerval __TIMER = {{0, 0}, {0, 100}};


/** wrapper function for passed entry function **/
static void real_entry(void*a_unused, void*b_unused, void* arg, sugare_entry_t callback, uint32_t ID)
{
    int result = callback(arg);
    __EXECUTED--;
    __RET_ADDRESS[ID] = result;
    __TERMINATED[ID] = true;
    if(!__EXECUTED) longjmp(__JMP_BUFFER, 1);

    while(1);
}


/** switch context on other stack frame **/
void switch_thread(struct Context ** ctx_from, struct Context * ctx_to)
{
    void __switch_threads(void ** prev, void * next);
    __switch_threads((void **)ctx_from, (void *)ctx_to);
}


/** allocate memory and initialize context first time **/
struct Context * create_thread(uint64_t stack_size, sugare_entry_t entry, void ** free_ptr, void * arg)
{
    uint64_t ctx_size = sizeof(struct Context);
    void * stack = malloc(stack_size + ctx_size);
    assert(stack);
    free_ptr = stack;

    struct Context context;
    memset(&context, 0, ctx_size);
    context.rip = (uint64_t)real_entry;
    context.rdx = (uint64_t)arg;
    context.rcx = (uint64_t)entry;
    context.r8 = (uint64_t)__TOP;
    memcpy(stack + stack_size, &context, ctx_size);

    return stack + stack_size;
}


/** free allocated memory **/
void destroy_thread(uint64_t stack_size, void * free_ptr)
{
    free(free_ptr);
}


/** add new thread in pool threads, return ID thread for check returs status **/
int push_thread(sugare_entry_t entry, void * arg)
{
    __POOL_THREADS[__TOP] = create_thread(__STACK_SIZE, entry, __FREE_PTRS[__TOP], arg);
    return __TOP++;
}


/** generate alarm signals for yourself on purpose switch threads **/
static void interrupt(int sig)
{
    static uint64_t prev_thread = 0;
    static uint64_t next_thread = 1;

    static uint64_t prev_thread_copy = 0;
    static uint64_t next_thread_copy = 0;

    static uint32_t i;

    if(__EXECUTED == 1)
    {
        if(!__TERMINATED[prev_thread % __TOP]) return;
        for(i = 0; i < __TOP; i++)
        {
            if(!__TERMINATED[i])
            {
                prev_thread_copy = prev_thread_copy;
                next_thread_copy = i;
                prev_thread_copy = i;
                switch_thread(&__POOL_THREADS[prev_thread_copy], __POOL_THREADS[next_thread_copy]);
                break;
            }
        }
    }

    /* or executing one thread or other threads have been terminated */
    if(__EXECUTED > 1)
    {
        while(__TERMINATED[next_thread % __TOP]) next_thread++;

        prev_thread_copy = prev_thread % __TOP;
        next_thread_copy = next_thread % __TOP;

        prev_thread = next_thread;
        next_thread++;

        setitimer(ITIMER_REAL, &__TIMER, NULL);
        switch_thread(&__POOL_THREADS[prev_thread_copy], __POOL_THREADS[next_thread_copy]);
    }
}


/** run main loop of switching contexts **/
int * run_scheduler(int * n)
{
    /* initialize signal handler */
    static struct sigaction action;
    action.sa_handler = &interrupt;
    action.sa_flags = SA_NODEFER;
    sigaction(SIGALRM, &action, NULL);

    if(__TOP > 0)
    {
        /* create noop context */
        struct Context scheduler;
        memset(&scheduler, 0, sizeof(scheduler));
        struct Context * pscheduler = &scheduler;

        /* cnt executing threads */
        __EXECUTED = __TOP;

        /* point of returning from longjmp */
        if(!setjmp(__JMP_BUFFER))
        {
            /* run loop */
            setitimer(ITIMER_REAL, &__TIMER, NULL);
            switch_thread(&pscheduler, __POOL_THREADS[0]);
        }

        /* retrieve start state */
        memset(__TERMINATED, 0, __TOP*sizeof(bool));

        /* destroy all contexts */
        for(uint32_t i = 0; i < __TOP; i++)
            destroy_thread(__STACK_SIZE, __FREE_PTRS[i]);
    }

    if(n != NULL) *n = __TOP;
    __TOP = 0;
    return __RET_ADDRESS;
}




