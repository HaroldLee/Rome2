/** @file sleep.S.
 *  @brief The implementation of sleep system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the sleep label so that they can be called from
 * other files (.c or .S) */
.global sleep

sleep:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $SLEEP_INT
    
    popl    %esi
    ret