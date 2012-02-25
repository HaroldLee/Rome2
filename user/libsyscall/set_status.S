/** @file set_status.S.
 *  @brief The implementation of set_status system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the set_status label so that they can be called from
 * other files (.c or .S) */
.global set_status

set_status:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $SET_STATUS_INT
    
    popl    %esi
    ret