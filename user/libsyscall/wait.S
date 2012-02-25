/** @file wait.S.
 *  @brief The implementation of wait system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the wait label so that they can be called from
 * other files (.c or .S) */
.global wait

wait:    
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $WAIT_INT
    
    popl %esi
    ret