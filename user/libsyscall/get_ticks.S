/** @file get_ticks.S.
 *  @brief The implementation of get_ticks system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the get_ticks label so that they can be called from
 * other files (.c or .S) */
.global get_ticks

get_ticks:
    INT $GET_TICKS_INT 
    
    ret