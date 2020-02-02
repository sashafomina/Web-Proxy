#include "dll_dict.h"
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

/* This is a doubley linked list dictionary, where each node
 * also has a timestamp. The keys are strings and the values are buffer_t 
 * pointers */

node_t *dll_get(node_t *head, char *key) {
  node_t *curr_node = head;
  while (curr_node != NULL) {
    /* Found node with matching key */
    if (strcmp(curr_node->key, key) == 0) {
      return curr_node;
    }
    curr_node = curr_node->next;
  }
  return NULL; 
}

/* Allocates memory for a new node_t and initialized 
 * the key, value ,and timestamp to the given quantities.
 * Only used inside the add() function*/
static node_t *create_node(char *key, buffer_t *value,
			   uint64_t timestamp) {
  node_t *new_node = malloc(sizeof(node_t));
  new_node->key = key;
  new_node->value = value;
  new_node->timestamp = timestamp;
  return new_node;
}

/* Adds the new node to the front of the linked list dictionary */
node_t *dll_add(node_t *head, char *key, buffer_t *value,
		uint64_t timestamp) {
  
  node_t *new_node = create_node(key, value, timestamp);

  if (head) {
    head->prev =  new_node;
    new_node->next = head;
    head = new_node;
  }
  /* If dict is empty */
  else {
    head = new_node;
    new_node->next = NULL;
  }
  
  new_node->prev = NULL;
  return head;
}


node_t *dll_remove(node_t *head, node_t *node) {
  node_t *prev_node = node->prev;
  node_t *next_node = node->next;

  if (prev_node) {
    prev_node->next = next_node;
  }
  /* If the nodes is the head node, then prev is null*/
  else {
    head = next_node;
  }
  if (next_node) {
    next_node->prev = prev_node;
  }
  
  /* No longer going to have a pointer to this node, must free*/
  if (node->value) {
    buffer_free(node->value);
  }
  free(node->key);
  free(node);
  
  return head;
}

void free_dll_dict(node_t *head) {
  node_t *curr_node = head;
  while (curr_node != NULL) {
    node_t *next_node = curr_node->next;
    /* Free the string key and buffer_t value befroe
     * freeing the node struct */
    free(curr_node->key);
    buffer_free(curr_node->value);
    free(curr_node);
    curr_node = next_node;
  }
}

