/** @file get_thread_from_stack.S
 *  @brief Get thread struture pointer from its stack.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
 
 /* define the get_thread_from_stack label so that they can be called from
  * other files (.c or .S) */
.global get_thread_from_stack
    
get_thread_from_stack:
    movl (%ebp), %edx # get the thread's origin %ebp from current 
                      #    function(do_thread())'s %ebp.
    movl (%edx), %eax # get the thread struture's information.
    
    ret
