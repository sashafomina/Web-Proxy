#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>


/* This cache is thread-safe, and approximately a least-recently used cache.
 * The underlying data structure is a seperate chaining hash dictionary, (doublely
 * linked lists in each bucket) to ensure approximately O(1) lookup. 
 * In order to maintain concurrency, read-write locks are used from get()
 * and add(). To find the least recently used node in the hashtable, each
 * node has a timestamp that was initialized when it was first added to the
 * cache and updated each time it is accessed by get(). The timestamps are
 * calculated using clock() and the node with the smallest clock is evicted.
 * This is not exactly LRU, because it it possible that two threads will
 * call clock() at similar times, so it won't store precisely the correct
 * time stamp as one of the times is overwritten. 
 * NOTE: see .h file for descriptive comments of eahc function 
 */
struct cache_t {
  /* number of bytes of web data not including meta data*/
  size_t num_bytes;
  pthread_rwlock_t rwlock;
  node_t *buckets[];
};

/* Allocates memory for the cache and initializes each bucket to NULL*/
cache_t *create_cache() {

  cache_t *cache =  malloc(sizeof(pthread_rwlock_t) + sizeof(size_t) +
			   sizeof(node_t *) * NUM_BUCKETS);
  assert(cache != NULL);

  /* Instantiating the cache */
  cache->num_bytes = 0;
  for (size_t i = 0; i < NUM_BUCKETS; i++) {
    /* Setting the head node of each bucket to NULL*/
    cache->buckets[i] = NULL;
  }
  pthread_rwlock_init(&(cache->rwlock), NULL);
  return cache;
}

/* Hashing function */
static size_t hash(char *key) {
  size_t sum = 0;
  for (ssize_t i = strlen(key)-1; i >=0; i--) {
    sum += key[i];
    sum *= 37;
  }
  return sum % NUM_BUCKETS;
}

/* Returns the least recently used node based on its timestamp in the cache
 * Only called from within add()*/
static node_t *find_LRU(cache_t *cache) {
  node_t *LRU_node = NULL; 
  for (size_t i = 0; i < NUM_BUCKETS; i++) {
    node_t *curr_node = cache->buckets[i];
    while (curr_node != NULL) {
      if (LRU_node == NULL) {
	LRU_node = curr_node;
      }
      
      if (curr_node->timestamp < LRU_node->timestamp) {
	LRU_node = curr_node;
      }
      curr_node = curr_node->next;
    }
  }
  return LRU_node;
}

/* Removing a node from the cache */
static void cache_remove(cache_t *cache,  node_t *node) {
  size_t hashed_key = hash(node->key);
  node_t *bucket = cache->buckets[hashed_key];
  cache->num_bytes -= buffer_length(node->value);
  cache->buckets[hashed_key] = dll_remove(bucket, node);

}

buffer_t *cache_get(cache_t *cache, char *key) {
  /* Reader lock */
  pthread_rwlock_rdlock(&(cache->rwlock));
  size_t hashed_key = hash(key); 
  node_t *bucket = cache->buckets[hashed_key];
  node_t *got_node = dll_get(bucket, key);
  
  if (got_node) {
    /* Update timestamp */
    got_node->timestamp = (uint64_t) clock();
    /* Get copy of value of the node to return */
    buffer_t *got_value_cpy = buffer_copy(got_node->value);
    pthread_rwlock_unlock(&(cache->rwlock));
    return got_value_cpy;
  }
  pthread_rwlock_unlock(&(cache->rwlock));
  return NULL;

}


void cache_add(cache_t *cache, char *key, buffer_t *value) {
  /* Writer lock*/
  pthread_rwlock_wrlock(&(cache->rwlock));
  /* If cache filled, must evict least recently used */
  while (cache->num_bytes + buffer_length(value) > MAX_CACHE_SIZE) {
    node_t *LRU_node = find_LRU(cache);
    /* Remove is only called from inside add */
    cache_remove(cache, LRU_node);
  }
  
  size_t hashed_key = hash(key);
  node_t *bucket = cache->buckets[hashed_key];
  /* Calculated timestamp */
  uint64_t timestamp = clock();
  cache->buckets[hashed_key] = dll_add(bucket, key, value, timestamp);
  /* Update number of data bytes in cache */
  cache->num_bytes += buffer_length(value);
  pthread_rwlock_unlock(&(cache->rwlock));
}



void print_cache(cache_t *cache) {
  for (size_t i = 0; i < NUM_BUCKETS; i++) {
    node_t *curr_node = cache->buckets[i];
    printf("\n");
    if (curr_node != NULL) {
      printf ("Bucket %zu: ", i);
    
      while (curr_node != NULL) {
	printf("%s: %s ->", curr_node->key,
	       buffer_string(curr_node->value));
	curr_node = curr_node->next;
      }
      printf("NULL\n");
    }
  }
}


void free_cache(cache_t *cache) {
  for (int i = 0; i < NUM_BUCKETS; i++) {
    free_dll_dict(cache->buckets[i]);
  }
  free(cache);
}


