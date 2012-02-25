/** @file exec.S.
 *  @brief The implementation of exec system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the exec label so that they can be called from
 * other files (.c or .S) */
.global exec

exec:    
  pushl    %esi
    
    /* call exec */
    leal 8(%esp),%esi
    INT $EXEC_INT
    
    popl %esi
    ret