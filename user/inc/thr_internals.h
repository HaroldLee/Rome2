/** @file thr_internals.h
 *
 *  @brief This file may be used to define things
 *         internal to the thread library.
 */
#ifndef THR_INTERNALS_H
#define THR_INTERNALS_H

#include <mutex.h>
#include <cond.h>
#include <def.h>

/* Thread status */
#define RUNNING 0
#define EXITED -1

typedef void *(*func_t)(void *);

/* Thread information struture */
typedef struct{
    int tid;
    void *stack_base;
    int stack_size; /* current stack size */

    int status;

    int join_thread;  /* joining thread, default is INVALID_THREAD */
    void *exit_status;

    mutex_t thr_mutex;
    cond_t exit_cond;

    func_t func;
    void * arg;
} thread_t;

/* Functions */
int init_thread_lib(unsigned int size);

thread_t *get_thread_by_tid(int tid);

thread_t *prepare_thread(void *(*func)(void *), void * arg);
void do_thread();
void prepare_thread_rollback(thread_t *thread);

void make_thread_running(int tid, thread_t *new_thread);
void exit_thread(thread_t *thread);
void reap_thread(thread_t *thread, int tid);

#endif /* THR_INTERNALS_H */
