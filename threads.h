#ifndef GT_H_THREADS
#define GT_H_THREADS

#include <inttypes.h>


/** main function in thread **/
typedef int(*sugare_entry_t)(void * arg);

/** add new thread in pool threads, return ID thread for check returs status **/
extern int              push_thread     (sugare_entry_t entry, void * arg);

/** run all threads in pool **/
extern int *            run_scheduler   (int * n);


#endif // GT_H_THREADS
