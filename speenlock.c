#include "speenlock.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>


/** 1) lock_one methods **/
static void init_lock_one(struct lock_one * lock_one)
{
    atomic_store(&lock_one->last, 0);
    for(unsigned i = 0; i < N; i++)
        atomic_store(&lock_one->flags[i], false);
}
static bool is_clear_flags(struct lock_one * lock_one, unsigned me)
{
    for(unsigned i = 1; i < N; i++)
        if(i != me && atomic_load(&lock_one->flags[i]))
            return false;
    return true;
}
static void lock_one(struct lock_one * lock_one, unsigned me)
{
    atomic_store(&lock_one->flags[me], true);
    atomic_store(&lock_one->last, me);

    while(!is_clear_flags(lock_one, me) && atomic_load(&lock_one->last) == me);
}
static void unlock_one(struct lock_one * lock, unsigned me)
{
    atomic_store(&lock->flags[me], false);
}


/** 2) lock methods **/
void _init_lock(struct lock * lock)
{
    for(unsigned i = 0; i < N - 1; i++)
        init_lock_one(&lock->locks[i]);
}
void _lock(struct lock * lock, unsigned me)
{
    for(unsigned i = 0; i < N - 1; i++)
        lock_one(&lock->locks[i], me);
}
void _unlock(struct lock * lock, unsigned me)
{
    for(unsigned i = N - 2; i >= 0; i--)
        unlock_one(&lock->locks[i], me);
}






