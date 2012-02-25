/** @file rwlcok_type.h
 *
 *  @brief read write lock realted structure and macros
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */


#ifndef _RWLOCK_TYPE_H
#define _RWLOCK_TYPE_H

#define INVALID_CNT -1

typedef struct rwlock {
    int rd_cnt;
    int wr_cnt;
    mutex_t rdcnt_mutex;
    mutex_t wrcnt_mutex;
    cond_t rd_cond;
    cond_t wr_cond;
	cond_t writer_cond;
} rwlock_t;

#endif /* _RWLOCK_TYPE_H */
