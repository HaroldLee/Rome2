/** @file cond_variable.c
 *
 *  @brief condition variable functions
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */
 
#include<stddef.h>
#include<def.h>
#include<cond_type.h>
#include<malloc.h>
#include<syscall.h>
#include<simics.h>

/** @brief init condition variables
 *  
 * This function should initialize the condition variable pointed to by cv.
 * The effects of using a condition variable before it has been initialized,
 * or of initializing it when it is already initialized and in use, 
 * are undefined. This function returns zero on success and a number less than 
 * zero on error.
 *
 * @param cv: condition variable
 * @return error or success
 **/
int cond_init(cond_t *cv)
{
    int ret = OK;
    
    if (NULL == cv)
        return ERROR;

    ret = mutex_init(&cv->condmutex);
    if (OK != ret)
        return ERROR;

    cv->conddestr = COND_DESTR_NO;
    linklist_init(&cv->condqueue);

    return OK;
}

/** @brief destroy condition variables
 *  
 * This function should "deactivate¡± the condition variable pointed to by cv.
 * It is illegal for an application to use a condition variable after it has
 * been destroyed (unless and until it is later re-initialized). It is illegal
 * for an application to invoke cond destroy() on a condition variable while
 * threads are blocked waiting on it.
 * 
 * @param cv: condition variable
 * @return none
 **/
void cond_destroy(cond_t *cv)
{
    /* tell other threads this cond has been destroyed, so don not use it */
    cv->conddestr = COND_DESTR_YES;
    
    while(1) {
        mutex_lock(&cv->condmutex);
        /* if the cond queue is not empty, wait until empty; otherwise destoy 
         * the cond variable
         */
        if (cv->condqueue.pstfirst != NULL) {
            mutex_unlock(&cv->condmutex);
            yield(-1);
        } else {  
            mutex_unlock(&cv->condmutex);
            mutex_destroy(&cv->condmutex);
            break;
        }
    }

    return;
}

/** @brief wait on a condition variable
 *  
 * The condition-wait function allows a thread to wait for a condition and
 * release the associated mutex that it needs to hold to check that condition.
 * The calling thread blocks, waiting to be signaled. The  blocked thread may
 * be awakened by a cond signal() or a cond broadcast(). Upon return from cond 
 * wait(), mp has been re-acquired on behalf of the calling thread.
 * 
 * @param cv: condition variable
 * @param mp: world lock
 * @return none
 **/
void cond_wait(cond_t *cv, mutex_t *mp)
{
    listnode_t *pnode = NULL;
    int flag          = 0;

    /* cond var has been destroyed, do not use it */
    if (COND_DESTR_YES == cv->conddestr)
        return;
    
    pnode = malloc(sizeof(listnode_t));
        
    /* init a node */
    pnode->pNext = NULL;
    pnode->data = (void *)gettid();

    mutex_lock(&cv->condmutex);

    /* Insert into queue */
    linklist_addtail(&cv->condqueue, pnode);

    /* release world mutex */
    mutex_unlock(mp);

   /* unlock queue */
    mutex_unlock(&cv->condmutex);

    deschedule(&flag);

    /* lock the world mutex again */
    mutex_lock(mp);

    return;
}

/** @brief dequeue the waiting threads
 *  
 * This function should wake up a thread waiting on the
 * condition variable pointed to by cv, if one exists.
 * 
 * @param cv: condition variable
 * @return none
 **/
void cond_signal(cond_t *cv)
{
    listnode_t *pnode = NULL;
    int tid = 0;
    
    /* lock queue */
    mutex_lock(&cv->condmutex);

    /* delete from the queue head */
    pnode = linklist_delhead(&cv->condqueue);
    if (NULL == pnode) {
        /* nobody in queue */
        mutex_unlock(&cv->condmutex);
        return;
    }

    /* unlock queue */
    mutex_unlock(&cv->condmutex);

    tid = (int)pnode->data;
    /* make a thread runnable */
    while (0 > make_runnable(tid))
        yield(tid);
    
    /* free node space */
    free(pnode);
        
    return;
}

/** @brief dequeue all waiting threads
 *  
 * This function should wake up all threads waiting on the condition variable
 * pointed to by cv. Note that cond broadcast() should not awaken threads
 * which may invoke cond wait(cv) "after¡± this call to cond broadcast()
 * has begun execution.
 * 
 * @param cv: condition variable
 * @return none
 **/
void cond_broadcast(cond_t *cv)
{
    listnode_t *pnode    = NULL;
    listnode_t *tmppnode = NULL;

    /* clear queue */
    mutex_lock(&cv->condmutex);
    pnode = linklist_delall(&cv->condqueue);
    mutex_unlock(&cv->condmutex);

    /* make runnable and free space one by one */
    while (NULL != pnode) {
        /* make it runnable until succeed */
        while (0 > make_runnable((int)pnode->data))
            yield((int)pnode->data);

        tmppnode = pnode;
        pnode = pnode->pNext;
        /* free node space */
        free(tmppnode);     
    }

    return;
}
