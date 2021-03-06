/** @file thread.c.
 *  @brief Thread Management API.
 *
 *
 *  1. How to seperate thread stack.
 *     We put a blank virtual memory page between every two threads' stack. The
 *     blank page will not be allocated to any thread. 
 *     When a thread go across its boundry, it may step in the thread stack 
 *     below and will corrupt other thread's data. If there is a blank page,
 *     it will cause a page fault. And the exception handler would find the 
 *     cross-border behavior and terminate the thread.
 *
 *     Another aspect is that we never put thread information(such as tid) on 
 *     its stack. Relying on the stack to store the information may spread error
 *	   from one thread to another thread. I will explain this in the 'How to 
 *     get tid section'.
 *
 *  2. Exception stack
 *     Each thread will be allocated with one exception stack(4K Byte) in the 
 *     front of its stack. Thus, the handler of different threads will execute
 *     on different stack.
 *
 *  3. Resource recycle
 *     When a thread exit, we will not release its stack immediately. We will
 *     put it into a free list. When new thread is created, we will find from 
 *     the free list first. 
 *
 *  4. How to get tid
 *     It is true that there are some quick way to get current thread's tid. 
 *     For example, put the tid on its stack. However, we will not use the 
 *     method. We do want the stack corruption error to spread to other thread.
 *
 *     Let me explain it with an example, suppose thread 7's stack is corrupted,
 *     and cause a fault. If the handler, use the tid from the stack, it will
 *     get some wrong answers, 12 maybe. As a result, the handler may try to
 *     terminate thread 12.
 *
 *     We want thr_gettid() always return with a right value.
 *
 *  5. Hash table
 *     We use a hash table to contain the threads information. As the thread 
 *     tid generated by the kernal will increase and not repeat. There will be
 *     less collision in the table. As a result, we expect to find one thread
 *     item with constant time.
 *  
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
 
/* -- Includes -- */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#include <thread.h>
#include <thr_internals.h>

#include <def.h>

#include <simics.h>

#define GET_STACK(thread) (((thread)->stack_base) - PAGE_SIZE - 3)

/* -- Imported Functions -- */
extern int thread_fork(void *, void *);


/** @brief Initialize the thread library.
 *
 *
 *  @param size the amount of stack space which will be available for use by 
 *  each thread.
 *  @return returns zero on success, and a negative number on error.
 */
int thr_init( unsigned int size )
{
    /* Initialize all the */
    return init_thread_lib(size);
}

/** @brief Creates a new thread to run func(arg).
 *
 *   Steps:
 *   1. Allocate a stack and a thread item(contain thread information)for
 *   the new thread.
 *   2. Invoke the thread fork system call. 
 *   3. Run the func(arg) in the child thread.
 *
 *  @param func the function to be run by the child thread.
 *  @param arg the arg fo the func.
 *  @return returns zero on success, and a negative number on error.
 */
int thr_create(void *(*func)(void *), void * arg)
{
    thread_t *new_thread;
    int ret;

    /*
     * Allocate a stack and a thread item.
     */
    new_thread = prepare_thread(func, arg);
    if(new_thread == NULL)
        return ERROR;

    /* 
     * Invoke the thread fork system call.
     */
    /* Child thread */
    if ((ret = thread_fork(GET_STACK(new_thread), (void *)new_thread)) == 0){
        /* Run the child thread. */
        do_thread();
    }
    /* Parent thread */
    else if(ret > 0){

        /* 
         * Thread start code 
         */
        new_thread->tid = ret;
        /* Make the child thread running */
        make_thread_running(ret, new_thread);
        mutex_lock(&new_thread->thr_mutex);
        new_thread->status = RUNNING;
        mutex_unlock(&new_thread->thr_mutex);

        /* Return child thread tid */
        return ret;
    }
    
    /* 
     * Error happened when fork new thread , recover error .
     */
    /* Recycle stack, new thread */
    prepare_thread_rollback(new_thread);
    
    return ERROR;
}

/** @brief Cleans up after a thread, optionally returning the status information
 *   provided by the thread at the time of exit..
 *
 *  @param tid the target thread
 *  @param statusp the value passed to thr exit() by the joined thread will be placed
 *  in the location referenced by statusp.
 *  @return returns zero on success, and a negative number on error.
 */
int thr_join( int tid, void **statusp )
{
    thread_t *thread;
    int self_tid;

    /* Join on the self, return error */
    self_tid = gettid();
    if(self_tid == tid)
        return ERROR;

    /* The thread is not created yet */
    thread = get_thread_by_tid(tid);
	lprintf("root addr %p\n", thread->stack_base);
    if(thread == NULL)   
        return ERROR;
    
    /*
     * Try to join the thread 
     */
    mutex_lock(&thread->thr_mutex);

    /* The thread is not joined, join it */
    if (thread->join_thread == INVALID_THREAD){
        thread->join_thread = self_tid;  
    }
    /* The thread is joined, return error */
    else{
        mutex_unlock(&thread->thr_mutex);
        return ERROR;
    }
	
    /* If the thread is not exited, wait for it to exit */
    if(thread->status != EXITED)
        cond_wait(&thread->exit_cond, &thread->thr_mutex);
    
    /* Get status */
    if(statusp != NULL)
        *statusp = thread->exit_status;
    mutex_unlock(&thread->thr_mutex);
	lprintf("get join %p %d\n", thread->stack_base, (int)*statusp);
    /* Reap the thread item */
    reap_thread(thread, tid);

    return OK;
}

/** @brief Exits the thread with exit status status.
 *
 *  If a thread other than the root thread returns from its body function 
 *  instead of calling thr exit(), the behavior should be the same as if the 
 *  function had called thr exit().
 *
 *
 *  @param status the exit status
 */
void thr_exit( void *status )
{
    thread_t *thread;
    int tid;

    /* Get the thread infomation */
    tid = gettid();
	lprintf("before get tid %d\n", tid);
    thread = get_thread_by_tid(tid);
	lprintf("get exit %d %p\n", tid, thread);
	
    /* Set status and exit status */
    mutex_lock(&thread->thr_mutex);
    thread->status = EXITED;
    thread->exit_status = status;
    mutex_unlock(&thread->thr_mutex);

	lprintf("exit %d %p %d\n", gettid(), thread->stack_base, (int)thread->exit_status);

    /* Clean up the thread resource and exit the thread */
    exit_thread(thread);
}

/** @brief Returns the thread ID of the currently running thread.
 *
 *  I use the gettid() system call to implement the function. It is true that 
 *  the method is slow, but it is the only way to make sure the function always 
 *  return the right value. See README.dox for some detail reason.
 *
 *  @return the thread ID of the currently running thread
 */
int thr_getid( void )
{
    return gettid();
}

/** @brief Defers execution of the invoking thread to a later time in favor of
 *  the thread with ID tid. 
 *
 *    If tid is -1, yield to some unspecified thread. If the thread with ID tid 
 *  is not runnable, or doesn��t exist, then an integer error code less than 
 *  zero is returned. Zero is returned on success.
 *
 *
 *  @param tid the thread to yield to.
 *  @return returns zero on success, and a negative number on error.
 */
int thr_yield( int tid )
{
    thread_t *thread;

    /* 
     * Check tid 
     */    
    if(tid > 0){
        thread = get_thread_by_tid(tid);
        /* no such running tid */
        if(thread == NULL)
            return ERROR;
    }
    else if(tid != -1){
        /* invalid tid value */
        return ERROR;
    }

    /* Yield may fail */
    if(yield(tid) < 0)
        return ERROR;
    else
        return OK;
}

