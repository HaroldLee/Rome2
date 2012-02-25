/** @file rwlock.c
 *
 *  @brief rwlock functions
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */

#include <syscall.h>
#include <def.h>
#include <stddef.h>
#include <mutex.h>
#include <cond.h>
#include <rwlock.h>

/** @brief init a rwlock
 *  
 * This function should initialize the lock pointed
 * to by rwlock. Effects of using a lock before it has been initialized may be 
 * undefined. This function returns zero on success and a number less than 
 * zero on error.
 * 
 * @para rwlock:
 * @return error or success
 **/
int rwlock_init(rwlock_t *rwlock)
{
      rwlock->rd_cnt= 0;
      rwlock->wr_cnt= 0;
      mutex_init(&rwlock->rdcnt_mutex);
      mutex_init(&rwlock->wrcnt_mutex);
      cond_init(&rwlock->rd_cond);
      cond_init(&rwlock->wr_cond);
      cond_init(&rwlock->writer_cond);

      return OK;
}

/** @brief destroy a rwlock
 *  
 * This function should ¡°deactivate¡± the lock
 * pointed to by rwlock.
 * It is illegal for an application to use a readers/writers lock after it has 
 * been destroyed (unless
 * and until it is later re-initialized). It is illegal for an application to 
 * invoke rwlock destroy()
 * on a lock while the lock is held or while threads are waiting on it.
 * 
 * @para rwlock:
 * @return none
 **/
void rwlock_destroy(rwlock_t *rwlock)
{
    mutex_destroy(&rwlock->rdcnt_mutex);
    mutex_destroy(&rwlock->wrcnt_mutex);
    cond_destroy(&rwlock->rd_cond);
    cond_destroy(&rwlock->wr_cond);
    cond_destroy(&rwlock->writer_cond);

    rwlock->rd_cnt= INVALID_CNT;
    rwlock->wr_cnt= INVALID_CNT;

    return;
}

/** @brief lock a rwlock
 *  
 * The type parameter is required to
 * be either RWLOCK READ (for a shared lock) or RWLOCK WRITE (for an exclusive 
 * lock). This
 * function blocks the calling thread until it has been granted the requested 
 * form of access.
 * 
 * @para rwlock:
 * @return none
 **/
void rwlock_lock(rwlock_t *rwlock, int type)
{
    switch (type){ 
	    case RWLOCK_WRITE: {
    		/* compete with other writers */
            mutex_lock(&rwlock->wrcnt_mutex);
            while(rwlock->wr_cnt > 0)
                cond_wait(&rwlock->wr_cond, &rwlock->wrcnt_mutex);

    		/* I win, and I get the writer lock, exclusively */
            rwlock->wr_cnt = 1;
            mutex_unlock(&rwlock->wrcnt_mutex);

    		/* check if there are readers, wait them to finish */
            mutex_lock(&rwlock->rdcnt_mutex);
            while(rwlock->rd_cnt > 0)
                cond_wait(&rwlock->writer_cond, &rwlock->rdcnt_mutex);
            
            mutex_unlock(&rwlock->rdcnt_mutex);

            break;
	    } case RWLOCK_READ: {
    		/* if there are writers, wait. Writers have priority */
            mutex_lock(&rwlock->wrcnt_mutex);
    		
            while(rwlock->wr_cnt > 0)
                cond_wait(&rwlock->rd_cond, &rwlock->wrcnt_mutex);

    	    /* else, get the reader lock */
            mutex_lock(&rwlock->rdcnt_mutex);
            rwlock->rd_cnt++;
            mutex_unlock(&rwlock->rdcnt_mutex);

    		/* unlock, after add the reader lock counter */
            mutex_unlock(&rwlock->wrcnt_mutex);

            break;
	    }
    } 
	/* other type value */
    return;
}

/** @brief unlock a rwlock
 *  
 * This function indicates that the calling
 * thread is done using the locked state in whichever mode it was granted access 
 * for. Whether
 * a call to this function does or does not result in a thread being awakened 
 * depends on the
 * situation and the policy you chose to implement.
 * It is illegal for an application to unlock a readers/writers lock that is not 
 * locked.
 * 
 * @para rwlock:
 * @return none
 **/
void rwlock_unlock(rwlock_t *rwlock) {
    
	/* unlock a reader lock */
    if (&rwlock->rd_cnt > 0) {
		mutex_lock(&rwlock->rdcnt_mutex);
        rwlock->rd_cnt--;

		/* no more readers, try to give writer the chance to step in */
        if (rwlock->rd_cnt == 0) 
            cond_signal(&rwlock->writer_cond);
		/* unlock */
        mutex_unlock(&rwlock->rdcnt_mutex);
        
	/* unlock a writer lock */
    } else {
    	mutex_lock(&rwlock->rdcnt_mutex);
        mutex_lock(&rwlock->wrcnt_mutex);

        rwlock->wr_cnt = 0;

		/* Signal writer that is waiting */
        cond_signal(&rwlock->wr_cond);
		
		/* Broadcast to readers that are waiting */
        cond_broadcast(&rwlock->rd_cond);

        mutex_unlock(&rwlock->wrcnt_mutex);
        mutex_unlock(&rwlock->rdcnt_mutex);      
    }
}

/** @brief unlock a rwlock
 *  
 * This function must be called by a thread that already owns the lock in
 * RWLOCK WRITE mode at a time when it no longer requires exclusive access
 * to the protected resource. When the function returns: no threads hold
 * the lock in RWLOCK WRITE mode; the invoking thread, and possibly some other 
 * threads, hold the lock in RWLOCK READ mode; previously blocked or newly
 * arriving writers must still wait for the lock to be released entirely.
 * During the transition from RWLOCK WRITE mode to RWLOCK READ mode the lock
 * should at no time be unlocked. This call should not block indefinitely.
 *
 * 
 * @para rwlock:
 * @return none
 **/
void rwlock_downgrade(rwlock_t *rwlock)
{
	mutex_lock(&rwlock->wrcnt_mutex);
    mutex_lock(&rwlock->rdcnt_mutex);

	/* downgrade */
	rwlock->wr_cnt --;
	rwlock->rd_cnt ++;
	
	/* Signal writer that is waiting */
	cond_signal(&rwlock->wr_cond);
	
	/* Broadcast to readers that are waiting */
	cond_broadcast(&rwlock->rd_cond);

	mutex_unlock(&rwlock->rdcnt_mutex);
    mutex_unlock(&rwlock->wrcnt_mutex);

    return;
}
