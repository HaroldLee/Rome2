/** @file get_cursor_pos.S.
 *  @brief The implementation of get_cursor_pos system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the get_cursor_pos label so that they can be called from
 * other files (.c or .S) */
.global get_cursor_pos

get_cursor_pos:
    pushl    %esi
    
    /* call get_cursor_pos */
    leal 8(%esp),%esi
    INT $GET_CURSOR_POS_INT  
    
    popl %esi
    ret