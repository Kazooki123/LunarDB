#ifndef NODE_H
#define NODE_H

typedef struct Node {
  int id;
  struct Node* next;
} Node;

Node* create_node(int id)
void free_node(Node* node)

#endif
