/** @file set_term_color.S.
 *  @brief The implementation of set_term_color system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the set_term_color label so that they can be called from
 * other files (.c or .S) */
.global set_term_color

set_term_color:
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $SET_TERM_COLOR_INT
    
    popl    %esi
    ret