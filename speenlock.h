#ifndef H_SPEENLOCK
#define H_SPEENLOCK

#include <stdbool.h>
#include <stdatomic.h>

#define N 30
struct lock_one {
    atomic_uint     last;
    atomic_bool     flags[N];
};
struct lock {
    struct lock_one locks[N-1];
};

/** n - count of threads **/
extern void _init_lock  (struct lock * lock);

/** me must be in range [1, n]**/
extern void _lock       (struct lock * lock, unsigned me);

extern void _unlock     (struct lock * lock, unsigned me);

#endif // H_SPEENLOCK
