/** @file readline.S.
 *  @brief The implementation of readline system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the readline label so that they can be called from
 * other files (.c or .S) */
.global readline

readline:
    pushl    %esi
    
    /* call readline */
    leal 8(%esp),%esi
    INT $READLINE_INT
    
    popl %esi
    ret