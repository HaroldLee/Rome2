/** @file %.h.
 *  @brief The implementation of hash table functions.
 *
 *  The data of each hash node is allocated and freed by the user. It can be 
 *  any user defines data struture.
 *
 *  @author Kaili Li (kailili)
 *  @author Qi Liu (qiliu)
 *  @bug No known bugs.
 */

#include <stdio.h>
#include <stdlib.h>

#include <def.h>

#include <hashtable.h>


/** @brief Create hash table.
 *
 *
 *  @param size hash table size
 *  @return hash table 
 */

hash_table_t *create_hash_table(int size)
{
    hash_table_t *hash_table;

    /* Hash Table, not use memory allocated by user */
    if((hash_table = malloc(sizeof(hash_table_t))) == NULL) 
        return NULL;

    /* Hash Table Nodes Header, use calloc here */
    if((hash_table->nodes = calloc(size, sizeof(hash_node_t*))) == NULL){
        free(hash_table);
        return NULL;
    }

    hash_table->size = size;
    hash_table->count = 0;

    return hash_table;
}


/** @brief Destroy hash table.
 *
 *
 *  @param hash_table hash_table to destroy
 *  @param free_data user define function to free the data, free() may be.
 *  @return 
 */
void destroy_hash_table(hash_table_t *hash_table, void (*free_data)(void *))
{
    int i;
    hash_node_t *node, *oldnode;
    
    for(i=0; i<hash_table->size; i++) {
        node = hash_table->nodes[i];
        while(node) {
            /* if necessary, I have to free the data here */
            free_data(node->data);

            oldnode = node;
            node = node->next;
            free(oldnode);
        }
    }
    
    free(hash_table->nodes);
    free(hash_table);
}

/** @brief Insert a (key, data) pair in the hash table.
 *
 *  Insert the key already in the hash table, it will return error, not update
 *  the hash table.
 *
 *  @param hash_table the hash table
 *  @param key  the key     
 *  @param data  the data
 *  @return 0 on success, negative if fail.
 */
int hash_table_insert(hash_table_t *hash_table, int key, void *data)
{
    hash_node_t *node;
    int hash_value;

    /* get the hash value */
    hash_value = key % hash_table->size;
    node=hash_table->nodes[hash_value];
    
    /* The hash item exist, return error */
    /* The implementation is only for the project */
    while(node){
        if(node->key == key)     
            return ERROR;
        node=node->next;
    }

    /*No such hash item, create one (in the list) */
    if((node = malloc(sizeof(hash_node_t))) == NULL)
        return ERROR;

    /*update the list here*/
    node->key = key;
    node->data= data;
    
    /* insert the node in front of the list */
    node->next = hash_table->nodes[hash_value];
    hash_table->nodes[hash_value] = node;
    hash_table->count ++;

    return OK;
}

/** @brief Delete an item in the hash table.
 *
 *
 *  @param hash_table the hash table
 *  @param key  the key    
 *  @return 0 on success, negative if fail.
 */

int hash_table_delete(hash_table_t *hash_table, int key)
{
    hash_node_t *node, *pre_node=NULL;
    int hash_value;

    /* get the hash value */
    hash_value = key % hash_table->size;
    pre_node = NULL;
    node = hash_table->nodes[hash_value];
    
    while(node) {
        if(node->key == key) {
            /* whether the node is the first node */
            if(pre_node != NULL)
                pre_node->next = node->next;
            else 
                hash_table->nodes[hash_value] = node->next;

            /* delete the item */
            /* data should already been freed */
            free(node);
            hash_table->count --;
            return OK;
        }
        pre_node = node;
        node = node->next;
    }

    /* not find the item */
    return ERROR;
}

/** @brief Search item with key in the hash table.
 *
 *
 *  @param hash_table the hash table
 *  @param key  the key    
 *  @return the hash item, NULL if fail. 
 */
void *hash_table_search(hash_table_t *hash_table, int key)
{
    hash_node_t *node;
    int hash_value;

    /* get the hash value */
    hash_value = key % hash_table->size;
    node=hash_table->nodes[hash_value];
    
    while(node) {
        /* find the item */
        if(node->key == key)
            return node->data;
        node=node->next;
    }

    /* not find the item */
    return NULL;
}


