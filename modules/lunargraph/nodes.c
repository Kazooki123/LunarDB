#include <stdlib.h>
#include "node.h"

Node* create_node(int id) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  if (!new_node) return NULL;
  new_node->id = id;
  new_node->next = NULL;
  return new_node;
}

void free_node(Node* node) {
  free(node);
}
