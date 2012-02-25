/** @file mutex.c.
 *  @brief The implementation of mutex.
 *
 *  How do you implement
 *  
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */

#include<syscall.h>
#include<def.h>
#include<stddef.h>
#include<mutex_type.h>

/* mutex has been destroyed or not */
#define MUTEX_DESTR_YES 1
#define MUTEX_DESTR_NO 0
     
/* mutex has been locked by a thread or not */
#define MUTEX_LOCK_YES 0
#define MUTEX_LOCK_NO 1

/* Atomic add or dec an integer */
#define ADD_LOCK_NUM(M_mutex) \
{\
    while (MUTEX_LOCK_NO != atom_xchg(&((M_mutex)->countlock), MUTEX_LOCK_YES))\
        yield(-1);\
    (M_mutex)->inmutex_count++;\
    (M_mutex)->countlock = MUTEX_LOCK_NO;\
}

#define DEC_LOCK_NUM(M_mutex) \
{\
    while (MUTEX_LOCK_NO != atom_xchg(&((M_mutex)->countlock), MUTEX_LOCK_YES))\
        yield(-1);\
    (M_mutex)->inmutex_count--;\
    (M_mutex)->countlock = MUTEX_LOCK_NO;\
}

/* Atomic test whether an integer is zero */
#define LOCK_NUM_TEST(M_mutex) \
{\
    while (MUTEX_LOCK_NO != atom_xchg(&((M_mutex)->countlock), MUTEX_LOCK_YES))\
        yield(-1);\
    if (0 != (M_mutex)->inmutex_count) { \
        (M_mutex)->countlock = MUTEX_LOCK_NO;\
        yield((M_mutex)->thread);\
    } \
    else{ \
        (M_mutex)->countlock = MUTEX_LOCK_NO;\
        break; \
    }\
}
 
extern int atom_xchg(int *, int);

/* used in malloc.c */
mutex_t malloc_thread_mutex = { .lock = MUTEX_LOCK_NO,
                                .thread = INVALID_THREAD, 
                                .destroy = MUTEX_DESTR_NO,
                                .inmutex_count = 0,
                                .countlock = MUTEX_LOCK_NO};

/** @brief Initialize a mutex.
 *
 *
 *    @param mp the mutex
 *    @return 0 on success, negative if fail
 */
int mutex_init(mutex_t *mp)
{  
    /* not lock */
    mp->lock = MUTEX_LOCK_NO;
    /* no thread get the mutex */
    mp->thread = INVALID_THREAD;
    
    /* mutex is initialize */
    mp->destroy = MUTEX_DESTR_NO;
    
    /* no thread is using or waiting mutex_lock */
    mp->inmutex_count = 0;
    mp->countlock = MUTEX_LOCK_NO;
    
    return OK;
}

/** @brief Destroy a mutex.
 *
 *
 *    @param mp the mutex
 *    @return 0 on success, negative if fail
 */
void mutex_destroy(mutex_t *mp)
{
    /* 
     * destroy the mutex and no one can step into mutex_lock from now on. 
     */
    mp->destroy = MUTEX_DESTR_YES;
    
    /* wait until no one is waiting or using the mutex_lock */
    while(1) {
        LOCK_NUM_TEST(mp);   
    }
}

/** @brief Lock a mutex.
 *
 *
 *    @param mp the mutex
 *    @return 0 on success, negative if fail
 */
void mutex_lock(mutex_t *mp)
{
    /* count the total number of threads who want to get the mutex */
    ADD_LOCK_NUM(mp);
    
    /* if the mutex has been destroyed, do not lock it */
    /* No need to recover the counter above, it is destroyed */
    if (MUTEX_DESTR_YES == mp->destroy)      
        return;

    /* try to accquire mutex */
    while (MUTEX_LOCK_NO != atom_xchg(&mp->lock, MUTEX_LOCK_YES)) 
        /* yield to the thread who own the mutex */
        yield (mp->thread);
    

    /* get the mutex */
    mp->thread = gettid();
            
    return;
}

/** @brief Unlock a mutex.
 *
 *
 *    @param mp the mutex
 *    @return 0 on success, negative if fail
 */
void mutex_unlock(mutex_t *mp)
{   
    mp->thread = INVALID_THREAD;
    
    /* the thread has finish using the mutex */
    DEC_LOCK_NUM(mp);

    mp->lock = MUTEX_LOCK_NO;

    return;
}
