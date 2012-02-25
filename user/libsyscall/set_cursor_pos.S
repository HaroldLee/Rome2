/** @file set_cursor_pos.S.
 *  @brief The implementation of set_cursor_pos system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the set_cursor_pos label so that they can be called from
 * other files (.c or .S) */
.global set_cursor_pos

set_cursor_pos:
    pushl    %esi
    
    /* call set_cursor_pos */
    leal 8(%esp),%esi
    INT $SET_CURSOR_POS_INT
    
    popl %esi
    ret