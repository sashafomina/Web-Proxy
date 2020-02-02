#ifndef DLL_DICT_H
#define DLL_DICT_H

#include <stddef.h>
#include <stdint.h>
#include "buffer.h"


typedef struct node_t {
  struct node_t *next;
  struct node_t *prev;
  char *key;
  buffer_t *value;
  uint64_t timestamp; 
} node_t;

/* Adds a node with the given field paramters to the front of the linked list
 * dictionary. 
 * and returns a pointer to the head node of the modified list */
node_t *dll_add(node_t *head, char *key, buffer_t *value,
		uint64_t timestamp);

/* Remove the node from the doubly linked list */
node_t *dll_remove(node_t *head, node_t *node);

/* Returns the node with the corresponding key through a linear search*/
node_t *dll_get(node_t *head, char *key);

/* Frees each node in the linked list dictionary and its data  */
void free_dll_dict(node_t *head);

#endif // DLL_DICT_H
