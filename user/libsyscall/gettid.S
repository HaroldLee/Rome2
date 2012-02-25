/** @file gettid.S.
 *  @brief The implementation of gettid system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the gettid label so that they can be called from
 * other files (.c or .S) */
.global gettid

gettid:
    INT $GETTID_INT

    ret