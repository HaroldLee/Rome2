/** @file fork.S.
 *  @brief The implementation of fork system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the fork label so that they can be called from
 * other files (.c or .S) */
.global fork

fork:
    INT $FORK_INT
    ret