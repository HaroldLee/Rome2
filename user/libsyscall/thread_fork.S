/** @file thread_fork.S.
 *  @brief The implementation of thread_fork system call.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#include<syscall_int.h>

/* define the thread_fork label so that they can be called from
 * other files (.c or .S) */
.global thread_fork
    
thread_fork:
    pushl %ebx  
    movl 4(%esp),%ecx   # %ecx is caller saved, we use it to save the retun address 
    movl 8(%esp),%edx   # %edx is caller saved, we use it to save parameter to child thread 
    movl 12(%esp),%ebx  # thread struture pointer
  
    INT $THREAD_FORK_INT
    testl %eax,%eax
    
    je .childthread   # return is not zero
    popl %ebx
    ret
    
.childthread:
    movl %edx,%ebp # setup stack for child thread
    movl %ebp,%esp
    
    movl %ebx, (%esp) # put the pointer to thread informatino on the stack
    
    pushl %ecx #push the return address

    ret
