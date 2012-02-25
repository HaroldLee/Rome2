/** @file halt.S.
 *  @brief The implementation of halt system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the halt label so that they can be called from
 * other files (.c or .S) */
.global halt

halt:
    pushl    %esi
    
    /* call halt */
    leal 8(%esp),%esi
    INT $HALT_INT
    
    popl %esi
    ret