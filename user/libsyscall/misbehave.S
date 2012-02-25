/** @file misbehave.S.
 *  @brief The implementation of misbehave system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the misbehave label so that they can be called from
 * other files (.c or .S) */
.global misbehave

misbehave:    
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $MISBEHAVE_INT
    
    popl    %esi
    ret