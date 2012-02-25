/** @file make_runnable.S.
 *  @brief The implementation of make_runnable system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the make_runnable label so that they can be called from
 * other files (.c or .S) */
.global make_runnable

make_runnable:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $MAKE_RUNNABLE_INT
    
    popl    %esi
    ret