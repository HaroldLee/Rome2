/** @file deschedule.S.
 *  @brief The implementation of deschedule system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the deschedule label so that they can be called from
 * other files (.c or .S) */
.global deschedule

deschedule:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $DESCHEDULE_INT
    
    popl    %esi
    ret