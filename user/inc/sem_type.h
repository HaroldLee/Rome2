/** @file linklist.h
 *
 *  @brief semaphore realted structure and macros
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H

#include <mutex_type.h>
#include <cond_type.h>

#define SEM_DESTR_NO 0
#define SEM_DESTR_YES 1

typedef struct sem {
    int count;
    mutex_t mutex;
    cond_t cond;
    int destr;
} sem_t;

#endif /* _SEM_TYPE_H */
