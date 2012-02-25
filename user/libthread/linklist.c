/** @file linklist.c
 *
 *  @brief all linklist operations
 *
 *  @author Qi Liu   (ID: qiliu)
 *  @author Kaili Li (ID: kailili)
 */
 
#include<linklist.h>
#include<stddef.h>

/** @brief init a linklist before use
 *  
 *
 * @param plist: linklist
 * @return none
 **/
void linklist_init(linklist_t *plist) 
{
    plist->pstfirst = NULL;
    plist->pstlast = NULL;

    return;
}

/** @brief delete a node from the head of the linklist
 *  
 *
 * @param plist: linklist
 * @return the pointer to deleted node
 **/
listnode_t* linklist_delhead(linklist_t* plist)
{
    listnode_t* pnode = plist->pstfirst;

    if(NULL != pnode)
        plist->pstfirst = pnode->pNext;
    
    if(NULL == plist->pstfirst)
        plist->pstlast = NULL;

    return pnode;
}

/** @brief add a node from the tail of the linklist
 *  
 *
 * @param plist: linklist
 * @return none
 **/
void linklist_addtail(linklist_t *plist, listnode_t *pnode)
{   
    if(NULL != plist->pstlast) {
        plist->pstlast->pNext = pnode;
        plist->pstlast = pnode; 
    } else {
        plist->pstlast = pnode;
        plist->pstfirst = pnode; 
    }

    return;
}

/** @brief delete all nodes from a linke list
 *  
 *
 * @param plist: linklist
 * @return pointer to the head node of this linklist
 **/
listnode_t* linklist_delall(linklist_t *pdellist)
{
    listnode_t*pnode = pdellist->pstfirst;
    
    pdellist->pstfirst = NULL;
    pdellist->pstlast = NULL; 

    return pnode;
}