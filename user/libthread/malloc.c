/** @file mallo.c
 *  @brief The implementation of malloc function family.
 *
 *  These functions should be thread safe. I use a mutex to protect the 
 *  functions.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>
#include <mutex.h>
#include <syscall.h>

#include <simics.h>

/* declare in mutex.c */
extern mutex_t malloc_thread_mutex;

/** @brief malloc() function.
 *
 *
 *  @param size the size of the allocate memory
 *  @return the address of the memory
 */
void *malloc(size_t size)
{
    void *tmp = NULL;
  
    mutex_lock(&malloc_thread_mutex);
    tmp = _malloc(size);
    mutex_unlock(&malloc_thread_mutex);

    return tmp;
}

/** @brief calloc() function.
 *
 *
 *  @param nelt the number of elt
 *  @param eltsize the size of the elt
 *  @return the address of the memory
 */
void *calloc(size_t nelt, size_t eltsize)
{
    void *tmp = NULL;
  
    mutex_lock(&malloc_thread_mutex);
    tmp = _calloc(nelt, eltsize);
    mutex_unlock(&malloc_thread_mutex);

    return tmp;
}

/** @brief realloc() function.
 *
 *
 *  @param buf the address of the allocated memory
 *  @param size the size of the allocate memory
 *  @return the address of the reallocated memory
 */
void *realloc(void *buf, size_t new_size)
{
    void *tmp = NULL;
  
    mutex_lock(&malloc_thread_mutex);
    tmp = _realloc(buf, new_size);
    mutex_unlock(&malloc_thread_mutex);

    return tmp;
}

/** @brief free() function.
 *
 *  @param buf the address of the allocated memory  
 */
void free(void *buf)
{
    mutex_lock(&malloc_thread_mutex);
    _free(buf);
    mutex_unlock(&malloc_thread_mutex);
  
    return;
}
