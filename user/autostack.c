/* If you want to use assembly language instead of C,
 * delete this autostack.c and provide an autostack.S
 * instead.
 */
#include<autostack.h>
#include<ureg.h>
#include<simics.h>
#include<def.h>
#include<autostack.h>
#include<syscall.h>
#include<stddef.h>
#include<thread.h>
#include<mutex.h>
#include<thr_internals.h>


#define PAGE_ALIGN_TEST ((unsigned int) (PAGE_SIZE-1))
#define PAGE_ALIGN_MASK ((unsigned int) ~((unsigned int) (PAGE_SIZE-1)))
#define ROOT_HDLR_STACK 0x011FFFFF

static int g_root_tid;
static void excpetion_handler(void *, ureg_t *);
static int root_stack_extend(void *);
static int thread_stack_extend(void *, thread_t *);

void install_autostack(void *stack_high, void *stack_low)
{
    /* check page alignment */
    if (0 != ((unsigned int)stack_low & PAGE_ALIGN_TEST))
        thr_exit((void *)-1);

    g_stackinfo.rootstack_low = stack_low;
    g_stackinfo.rootstack_hi = stack_high;
    g_stackinfo.max_stacksize = (int)stack_high - ROOT_HDLR_STACK;
    g_root_tid = gettid();

    /* space for the stack of root thread */
    if (0 > new_pages((void *)(ROOT_HDLR_STACK - PAGE_SIZE + 1), PAGE_SIZE))
        thr_exit((void *)-1);

    /* register a exception handler */
    register_exception_handler((void *)ROOT_HDLR_STACK + 1, NULL);

    return;
}

void register_exception_handler(void *stackbase, ureg_t *ureg)
{
    swexn(stackbase, excpetion_handler, NULL, ureg);

    return;
}

void excpetion_handler(void *arg, ureg_t *ureg)
{
    void *faultaddr = NULL;
    void *phdlrstack = NULL;
    int ret = ERROR;
    thread_t *pthread = NULL;
    int tid = gettid();
    
    /* if something rather than page fault */
    if ((NULL == ureg) || (SWEXN_CAUSE_PAGEFAULT != ureg->cause) ||
        (0 != ((ureg->error_code) & 1)))
        thr_exit((void *)-1);

    /* get the fault address */
    faultaddr = (void *)ureg->cr2;

    if (g_root_tid == tid) {
        phdlrstack = (void *)ROOT_HDLR_STACK + 1;
        
        /* root thread extends stack */
        ret = root_stack_extend(faultaddr);             
    } else {
        /* get the thread from hash table */
        pthread = get_thread_by_tid(tid);
        if (NULL == pthread)
            thr_exit((void *)-1);
        phdlrstack = pthread->stack_base + 1;
        
        /* other threads extend stack */
        ret = thread_stack_extend(faultaddr, pthread);     
    }
   
    /* no more space to extend, terminate this thread */
    if (OK != ret) {
        lprintf("+++++++++++++ tid%d ret err++++++++++++\n", tid);
        thr_exit((void *)-1);
    }
    
    /* re install handler */
    register_exception_handler(phdlrstack, ureg);
    
    return;
}

int root_stack_extend(void *faultaddr)
{
    void *pagebase = NULL;
    int extendsize = 0;
    thread_t *pthread = NULL;

    lprintf("%d in root_stack_extend\n", gettid());

    /* faul address is out of the range of stack extension */
    if ((faultaddr >= g_stackinfo.rootstack_low) || 
        (faultaddr <= (g_stackinfo.rootstack_hi - g_stackinfo.max_stacksize)))
        return ERROR;

    pagebase = (void *)((unsigned int)faultaddr & PAGE_ALIGN_MASK);
    extendsize = (int)(g_stackinfo.rootstack_low - pagebase);

    /* new_page for thread stack */
    if(0 > new_pages(pagebase, extendsize))
        return ERROR;

    /* update thread info */
    g_stackinfo.rootstack_low = pagebase;

    /* 
     * also record in root thread structure because root may terminate and this
     * stack sapce can be used again
     */
    if (LIB_IS_INIT == g_stackinfo.is_init) {
        pthread = get_thread_by_tid(g_root_tid);
        if (NULL == pthread)
            return ERROR;
        pthread->stack_size = g_stackinfo.rootstack_hi - 
                              g_stackinfo.rootstack_low;
    }
    
    return OK;
}

int thread_stack_extend(void *faultaddr, thread_t *pthread)
{
    void *pagebase = NULL;
    int extendsize = 0;
    
    /* faul address is out of the range of stack extension */
    if ((faultaddr > (pthread->stack_base - pthread->stack_size)) ||
        (faultaddr <= (pthread->stack_base - g_stackinfo.max_stacksize))) {
        lprintf("++++++++++ tid%d out of the range ++++++++++\n", pthread->tid);
        return ERROR;
    }

    pagebase = (void *)((unsigned int)faultaddr & PAGE_ALIGN_MASK);
    extendsize = (int)((char *)pthread->stack_base - pthread->stack_size
                       + 1 - (char *)pagebase);

    /* new_page for thread stack */
    if(0 > new_pages(pagebase, extendsize)) 
        return ERROR;
    
    /* update thread info */
    pthread->stack_size += extendsize;

    return OK;
}
