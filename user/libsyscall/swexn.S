/** @file swexn.S.
 *  @brief The implementation of swexn system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the swexn label so that they can be called from
 * other files (.c or .S) */
.global swexn

swexn:
    pushl    %esi
    
    /* call swexn */
    leal 8(%esp),%esi
    INT $SWEXN_INT
    
    popl %esi
    ret