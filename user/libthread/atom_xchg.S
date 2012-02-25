/** @file atom_xchg.S
 *  @brief The implementation of atomic xchg in assembly language.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
 
 /* define the atom_xchg label so that they can be called from
  * other files (.c or .S) */
.global atom_xchg

atom_xchg:
  movl 8(%esp),%eax  /* source */
  movl 4(%esp),%ecx
  XCHG (%ecx),%eax  /* exchange */
  ret  
  
  