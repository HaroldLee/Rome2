/** @file new_pages.S.
 *  @brief The implementation of new_pages system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the new_pages label so that they can be called from
 * other files (.c or .S) */
.global new_pages

new_pages:
    pushl    %esi
    
    /* call new_pages */
    leal 8(%esp),%esi
    INT $NEW_PAGES_INT
    
    popl %esi
    ret