/** @file yield.S.
 *  @brief The implementation of yield system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the yield label so that they can be called from
 * other files (.c or .S) */
.global yield

yield:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $YIELD_INT
    
    popl    %esi
    ret