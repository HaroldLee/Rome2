/** @file thr_internals.c.
 *  @brief functions internal to the thread library.
 *
 *  The file are the helper functions that with by used by the thread library 
 *  and autostack handler.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */

/* -- Includes -- */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#include <thr_internals.h>
#include <linklist.h>
#include <hashtable.h>
#include <thread.h>
#include <autostack.h>

#include <def.h>

#include <simics.h>

/* -- Macro Definition --*/

/* the default siz of the hash table (the max hash value) */
#define HASH_TABLE_SIZE 512

/* make the size page-aligned  */
#define ALIGN_PAGE_SIZE(size) (((size) + PAGE_SIZE - 1) & 0xfffff000)

/* the thread library information */
typedef struct{
    int is_init;

    int stack_size_max; /* Default stack size */
    int root_tid;  

    int thread_nums;    /* Threads number */
    mutex_t thread_nums_mutex;

    /* hash table to contain threads' information */
    hash_table_t *threads;
    mutex_t hash_table_mutex;

    /* linked list to recycle exited thread resource */
    linklist_t free_stack_list;
    void *current_base;    
    mutex_t link_list_mutex;
    
} thread_lib_t;

/* -- Local Variables -- */
extern stackinfo_t g_stackinfo;    /* import from autostack */
static thread_lib_t thread_lib;


/* -- Local Functions -- */
extern void *get_thread_from_stack();

static void * create_user_stack();
static thread_t *create_thread_item(void *base);

static void put_to_free_list(thread_t *thread);
static thread_t *find_free_thread();

/** @brief Initialize the thread library.
 *
 *  Do most of the work of thr_init().
 *
 *  @param size default size of each thread created
 *  @return 
 */
int init_thread_lib(unsigned int size)
{
    thread_t *tmp;
        
    /* 
     * Set the max stack size  
     * Note: one extra PAGE_SIZE space is for the software exception handler 
     */
    thread_lib.stack_size_max = ALIGN_PAGE_SIZE(size)  + PAGE_SIZE;
    g_stackinfo.max_stacksize = thread_lib.stack_size_max;

    /* Set root tid */
    thread_lib.root_tid = gettid();

    /* 
     * Set stack number 
     * 1. create root threads 
     */
    tmp = create_thread_item(g_stackinfo.rootstack_hi);
    if(tmp == NULL)
        return ERROR;

    tmp->tid = thread_lib.root_tid;
	tmp->status = RUNNING;
    
    thread_lib.thread_nums = 1;
    mutex_init(&thread_lib.thread_nums_mutex);
     
    /* 
     * Set hash table
     * 1. contain root thread structure */
    thread_lib.threads = create_hash_table(HASH_TABLE_SIZE);
    if(thread_lib.threads == NULL)
        return ERROR;
    
    hash_table_insert(thread_lib.threads, thread_lib.root_tid, (void *)tmp);
    mutex_init(&thread_lib.hash_table_mutex);

    /*
     * Set current_base according to root thread's stack information. 
     */
    tmp->stack_size = (g_stackinfo.rootstack_hi-g_stackinfo.rootstack_low+1);
	
    /*
     * Root thread may have been allocate with space more than 
     * default stack size, adjust the current_base.
     */
    if(thread_lib.stack_size_max <= (tmp->stack_size + PAGE_SIZE))
        thread_lib.current_base = g_stackinfo.rootstack_low + 
            thread_lib.stack_size_max - 1 - PAGE_SIZE; 
    else
        thread_lib.current_base = g_stackinfo.rootstack_hi;

    /* 
     * Set the free stack list 
     */
    linklist_init(&thread_lib.free_stack_list);
    mutex_init(&thread_lib.link_list_mutex);

    /*
     * Set library as inited.
     */
    g_stackinfo.is_init = LIB_IS_INIT;
    thread_lib.is_init = LIB_IS_INIT;

    return OK;
}

/** @brief Prepare resource for the new thread to run (stack, thread structure).
 *
 *    First, try to find a thread structure from the free list which contains recycled
 *  thread resource.
 *  
 *  If the free list is empty, allocate new thread structure and stack resource.
 *
 *  @param func the function to be run by the child thread.
 *  @param arg the arg fo the func.
 *  @return thread structure
 */
thread_t  *prepare_thread(void *(*func)(void *), void * arg)
{
    void *new_stack;
    thread_t *new_thread;
    
    /* Try to find a thread structure on the free list */
    new_thread = find_free_thread();

    /* Not find a thread structure, allocate new one */
    if(new_thread == NULL){
        /* Create a thread structure */
        new_stack = create_user_stack();
        if(new_stack == NULL)
            return NULL;

        /* Create a thread structure  */
        new_thread = create_thread_item(new_stack);
        /* cannot rollback here */
        if(new_thread == NULL)
            return NULL;
    }

    /* Set the func and arg to the thread structure */
    new_thread->func = func; 
    new_thread->arg = arg;
    
    return new_thread;
}

/** @brief Roll back what prepare_thread() have done.
 *
 *  Call the function when thread_fork() fail.
 *
 *  @param thread thread structure returned by prepare_thread() 
 */
void prepare_thread_rollback(thread_t *thread)
{    
    /* put the thread into free list */
    put_to_free_list(thread);
}

/** @brief Run the thread with func(arg).
 *
 *  Wait until the parent make the thread RUNNING, the run func(arg).
 *
 */

void do_thread()
{
    thread_t *tmp;
    void *status;

    /*
     * Get the thread struture from the stack. 
     */
    tmp = (thread_t *)get_thread_from_stack();

    /* Register exception handler */
    register_exception_handler(tmp->stack_base + 1, NULL);

    /* Wait for parent thread make me runnable */
    while(tmp->status != RUNNING)
        yield(-1);

    /*
     * Start thread here 
     */
    status = tmp->func(tmp->arg);

    /* If func return, exit thread here */
    thr_exit(status);
}

/** @brief Get thread by its tid.
 *
 *  @param tid the target thread's tid.
 *  @return the targe thread, NULL if not existed.
 */

thread_t *get_thread_by_tid(int tid)
{
    thread_t *tmp;
    
    /* Search in the hash table */
    mutex_lock(&thread_lib.hash_table_mutex);
    tmp = hash_table_search(thread_lib.threads, tid);
    mutex_unlock(&thread_lib.hash_table_mutex);

    return tmp;
}

/** @brief Make a thread running.
 *
 *  Put the thread structure into the hash table, and add thread_nums by 1.
 *
 *  @param tid the target thread's tid.
 *  @return new_thread the targe thread. 
 */

void make_thread_running(int tid, thread_t *new_thread)
{
    /* 
     * Put in the hash table 
     */
    mutex_lock(&thread_lib.hash_table_mutex);
    /* 
     * Note: Tid will be different for different threads, no need to check 
     * the return value. The caller of the function make sure there is no 
     * same tid. 
     */
    hash_table_insert(thread_lib.threads, tid, (void *)new_thread);
    mutex_unlock(&thread_lib.hash_table_mutex);

    /* Increase the thread number */
    mutex_lock(&thread_lib.thread_nums_mutex);
    thread_lib.thread_nums ++;
    mutex_unlock(&thread_lib.thread_nums_mutex);
}

/** @brief Reap the thread structure.
 *
 *    Called by thr_join().
 *
 *  @return thread the targe thread.
 *  @param tid the target thread's tid. 
 */
void reap_thread(thread_t *thread, int tid)
{    
    /* Delete from hash table */
    mutex_lock(&thread_lib.hash_table_mutex);
    hash_table_delete(thread_lib.threads, tid);
    mutex_unlock(&thread_lib.hash_table_mutex);

    /* 
     * Delete the thread 
     */
    mutex_destroy(&thread->thr_mutex);
    cond_destroy(&thread->exit_cond);

    free(thread);
}

/** @brief Exit the thread and recycle the resource.
 *
 *  Called by thr_exit().
 *
 *  @param ch the character to print
 *  @return 
 */
void exit_thread(thread_t *thread)
{
    thread_t *new_thread;

    /* Decrease the thread number and check if it is the last thread */
    mutex_lock(&thread_lib.thread_nums_mutex);
    thread_lib.thread_nums --;
    if(thread_lib.thread_nums == 0)
        set_status((int)thread->exit_status);
    mutex_unlock(&thread_lib.thread_nums_mutex);
    
    /* 
     * Make a copy of the exited thread to keep records of 
     * its stack information. 
     */
    new_thread = create_thread_item(thread->stack_base);

    new_thread->stack_size = thread->stack_size;
    new_thread->tid = INVALID_THREAD;

    /* Try to signal the joining thread, at most one joining thread */
    cond_signal(&thread->exit_cond);

    /* Put the resource(thread struture, stack) into free list */    
    put_to_free_list(new_thread);

    /* Exit thread */
    vanish();
}


/** @brief Create a new user stack according to current stack base 
 *
 *
 *  @return  stack top address with form 0x*****fff.
 */
static void * create_user_stack()
{
    void *page_addr;
    
    mutex_lock(&thread_lib.link_list_mutex);
    
    /* 
     * One more page to seperate the threads, the bland page will not be used 
     * by any thread. It seperate threads' stack space.
     */
    thread_lib.current_base -= (thread_lib.stack_size_max + PAGE_SIZE);
    
    page_addr = thread_lib.current_base - PAGE_SIZE*2 + 1;
    if (new_pages(page_addr, PAGE_SIZE * 2) < 0){
        /* 
         * The 'only' reason that new_page() fault is that is allocated, so it
         * is better NOT to recover current_base's value.
         */
        mutex_unlock(&thread_lib.link_list_mutex);
        return NULL;
    }
    
    mutex_unlock(&thread_lib.link_list_mutex);
    return thread_lib.current_base;
}


/** @brief Malloc a thread structure and initialize with default value.
 *
 *
 *  @param base the top stack address.
 *  @return the thread structure
 */     
static thread_t *create_thread_item(void *base)
{
    thread_t *tmp;

    if((tmp = malloc(sizeof(thread_t))) == NULL)
        return NULL;

    /* 
     * Set values 
     * No sync issues here
     */
    tmp->stack_base = base;
    /* One page for exception stack and one for user stack */
    tmp->stack_size = PAGE_SIZE * 2; 
    tmp->join_thread = INVALID_THREAD;
    tmp->exit_status = NULL;
    tmp->status = EXITED;

    /* Initailize mutex and conditional variable */
    mutex_init(&tmp->thr_mutex);
    cond_init(&tmp->exit_cond);
    
    return tmp;
}

/** @brief Put a thread structure into the free list.
 *
 *
 *  @param thread the target thread
 */

/* put an structure to the free list */
static void put_to_free_list(thread_t *thread)
{
    listnode_t *node;

    /* Put into the free list */
    node = malloc(sizeof(listnode_t));
    node->data = (void *)thread;
    node->pNext = NULL;

    mutex_lock(&thread_lib.link_list_mutex);
    linklist_addtail(&thread_lib.free_stack_list, node);
    mutex_unlock(&thread_lib.link_list_mutex);
}

/** @brief Find a thread structure from the free list.
 *
 *
 *  @return a thread stucture
 */
static thread_t *find_free_thread()
{
    listnode_t *node;
    thread_t *thread;

    /* Find from the free list */
    mutex_lock(&thread_lib.link_list_mutex);
    node = linklist_delhead(&thread_lib.free_stack_list);
    mutex_unlock(&thread_lib.link_list_mutex); 

    /* Not find an structure, return NULL */
    if(node == NULL)
        return NULL;

    /* Get the thread struture and free the node */
    thread = (thread_t *)node->data;
    free(node);
    
    return thread;
}


