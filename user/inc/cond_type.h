/** @file cond_type.h
 *
 *  @brief condition variable structure and macros
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

#include<mutex.h>
#include<linklist.h>

#define COND_DESTR_NO 0
#define COND_DESTR_YES 1

typedef struct cond_t {
    mutex_t condmutex;
    linklist_t condqueue;
    int conddestr;
} cond_t;

#endif /* _COND_TYPE_H */
