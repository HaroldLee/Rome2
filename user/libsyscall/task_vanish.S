/** @file task_vanish.S.
 *  @brief The implementation of task_vanish system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the task_vanish label so that they can be called from
 * other files (.c or .S) */
.global task_vanish

task_vanish:    
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $TASK_VANISH_INT
    
    popl %esi
    ret