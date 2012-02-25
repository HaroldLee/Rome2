/** @file getchar.S.
 *  @brief The implementation of getchar system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the getchar label so that they can be called from
 * other files (.c or .S) */
.global getchar

getchar:        
    INT $GETCHAR_INT
    
    ret