/** @file remove_pages.S.
 *  @brief The implementation of remove_pages system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the remove_pages label so that they can be called from
 * other files (.c or .S) */
.global remove_pages

remove_pages:    
    pushl    %esi
    
    movl 8(%esp),%esi  # Because of pushing %esi
    INT $REMOVE_PAGES_INT
    
    popl    %esi
    ret