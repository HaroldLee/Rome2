/** @file sem.c
 *
 *  @brief semaphore functions
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */

#include<def.h>
#include<sem_type.h>
#include<cond.h>
#include<syscall.h>

/** @brief init a semaphore
 *  
 * This function should initialize the semaphore pointed to by sem to the
 * value count. Effects of using a semaphore before it has been initialized
 * may be undefined. This function returns zero on success and a number less
 * than zero on error.
 * 
 * @param sem  : semaphore
 * @param count: initial value
 * @return error or success
 **/
int sem_init(sem_t *sem, int count)
{
    sem->count = count;
    sem->destr = SEM_DESTR_NO;
    
    if (OK != mutex_init(&sem->mutex))
        return ERROR;

    if (OK != cond_init(&sem->cond))
        return ERROR;
    
    return OK;
}

/** @brief destroy a semaphore
 * 
 * This function should ¡°deactivate¡± the semaphore pointed to by sem.
 * Effects of using a semaphore after it has been destroyed may be undefined.
 * It is illegal for an application to use a semaphore after it has been 
 * destroyed (unless and until it is later re-initialized). It is illegal for
 * an application to invoke sem destroy() on asemaphore while threads are
 * waiting on it.
 *
 * @param sem: semaphore
 * @return none
 **/
void sem_destroy(sem_t *sem)
{
    /* tell later threads do not use this sem */
    sem->destr = SEM_DESTR_YES;
    
    while(1) {
        mutex_lock(&sem->mutex);

        /* wiating until no threads wait for this sem, then destroy it */
        if (sem->count < 0) {
            mutex_unlock(&sem->mutex);
            yield(-1);
        } else {
            mutex_unlock(&sem->mutex);
            mutex_destroy(&sem->mutex);
            cond_destroy(&sem->cond);
            break;
        }
    }

    return;
}

/** @brief wait on a semaphore
 * 
 * The semaphore wait function allows a thread to decrement a semaphore value,
 * and may cause it to block indefinitely until it is legal to perform the
 * decrement.
 *
 * @param sem: semaphore
 * @return none
 **/
void sem_wait(sem_t *sem)
{
    /* this sem has been destroyed, so do not use it */
    if (sem->destr == SEM_DESTR_YES)
        return;
    
    /* lock the sem */
    mutex_lock(&sem->mutex);

    sem->count--;
    if (sem->count < 0)
        cond_wait(&sem->cond, &sem->mutex);
               
    /* unlock the sem */
    mutex_unlock(&sem->mutex);

    return;
}

/** @brief dequeue a thread waiting on semaphore
 * 
 * This function should wake up a thread waiting on the semaphore pointed to
 * by sem, if one exists, and should update the semaphore value regardless.
 *
 * @param sem: semaphore
 * @return none
 **/
void sem_signal(sem_t *sem)
{
    /* lock the sem */
    mutex_lock(&sem->mutex);
    
    sem->count++;

    /* awake some thread from the waiting queue */
    cond_signal(&sem->cond);

    /* unlock the sem */
    mutex_unlock(&sem->mutex);

    return;
}
