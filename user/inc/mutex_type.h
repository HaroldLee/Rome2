/** @file mutex_type.h.
 *  @brief The type definition of mutex
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

typedef struct mutex {
  int lock;
  int thread;
  int destroy;
  int inmutex_count;
  int countlock;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
