/** @file hashtable.h.
 *  @brief The .h file of the Hash Table functions. 
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */
#ifndef _HASH_TABLE_H
#define _HASH_TABLE_H

typedef struct hash_node {
    int key;
    void *data;
    struct hash_node *next;
} hash_node_t;

typedef struct {
    int size;
    int count;
    hash_node_t **nodes;
} hash_table_t;

/* create a hash table */
hash_table_t *create_hash_table(int size);

/* destroy the hash table */
void destroy_hash_table(hash_table_t *hash_table, void (*free_data)(void *));

/* insert a hash item */
int hash_table_insert(hash_table_t *hash_table, int key, void *data);

/* delete a hash item */
int hash_table_delete(hash_table_t *hash_table, int key);

/* search for a hash item with a key value */
void *hash_table_search(hash_table_t *hash_table, int key);

#endif /* _HASH_TABLE_H */

