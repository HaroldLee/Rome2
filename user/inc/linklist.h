/** @file linklist.h
 *
 *  @brief linklist realted functions and structures
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */

#ifndef __LINKLIST_H_
#define __LINKLIST_H_

/* node */
typedef struct listnode_t {
    struct listnode_t* pNext;
    void *data;
} listnode_t;

/* list head and tail */
typedef struct {
    listnode_t* pstfirst;
    listnode_t* pstlast;
} linklist_t;

/* functions */
void linklist_init(linklist_t *plist);
listnode_t* linklist_delhead(linklist_t* plist);
void linklist_addtail(linklist_t *plist, listnode_t *pnode);
listnode_t* linklist_delall(linklist_t *plist);

#endif
