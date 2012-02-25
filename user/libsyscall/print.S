/** @file print.S.
 *  @brief The implementation of print system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the print label so that they can be called from
 * other files (.c or .S) */
.global print

print:
    pushl    %esi
    
    /* call print */
    leal 8(%esp),%esi
    INT $PRINT_INT
    
    popl %esi
    ret