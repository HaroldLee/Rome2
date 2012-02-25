/** @file autostack.h.
 *  @brief The autostack handler definition.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
 
#ifndef _AUTOSTACK_H
#define _AUTOSTACK_H

#include<ureg.h>

/* Stack information */
typedef struct {
    void *rootstack_low;
    void *rootstack_hi;
    int max_stacksize;
    int is_init;
} stackinfo_t;

/* root stack information, modified by thread library */
stackinfo_t g_stackinfo;

void register_exception_handler(void *, ureg_t *);

#endif /* _AUTOSTACK_H */
