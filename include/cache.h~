#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <stdint.h>
#include "buffer.h"
#include "dll_dict.h"


#define MAX_CACHE_SIZE 1048576
#define MAX_OBJECT_SIZE 100000
#define NUM_BUCKETS 53

/* Nodes containined in the queue and hash table*/
typedef struct cache_t cache_t;

/* Allocates memory for the cache and initializes each bucket to NULL*/
cache_t *create_cache();

/* Returns a copy of the value of a node corresponding to a given key
 * as a buffer_t pointer. Updates the time stamp of node.
 * There is a read lock around the entire function. */
buffer_t *cache_get(cache_t *cache, char *key) ;


/* Adds a key and a value ot the cache, calculating its time stamp.
 * Increments the cache's total number of bytes field. If 
 * number of web data bytes in the cache exceeds a maximum amount
 * the least recently used node is evicted.
 * There is a write lock around this entire function.*/ 
void cache_add(cache_t *cache, char *key, buffer_t *value);

/*Goes through each node in the cach and free's all of its data,
 * then frees the cache struct itself*/
void free_cache(cache_t *cache) ;

/* Prints out the non NULL buckets and all the key value pairs
 * in that bucket */
void print_cache(cache_t *cache);

#endif //HASH_H
