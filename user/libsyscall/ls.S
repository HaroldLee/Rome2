/** @file ls.S.
 *  @brief The implementation of ls system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the ls label so that they can be called from
 * other files (.c or .S) */
.global ls

ls:
    pushl    %esi
    
    /* call ls */
    leal 8(%esp),%esi
    INT $LS_INT
    
    popl %esi
    ret