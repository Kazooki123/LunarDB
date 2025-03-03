#ifndef GRAPH_H
#define GRAPH_H

#include "nodes.h"

typedef struct Graph {
  int num_nodes;
  Node** adjacency_list;
} Graph;

Graph* create_graph(int num_nodes);
void add_edge(Graph* graph, int src, int dest);
void free_graph(Graph* graph);

#endif
