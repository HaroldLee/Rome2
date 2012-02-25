/** @file vanish.S.
 *  @brief The implementation of vanish system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the vanish label so that they can be called from
 * other files (.c or .S) */
.global vanish

vanish:        
    INT $VANISH_INT
    ret