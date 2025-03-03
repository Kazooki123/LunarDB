#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

Graph* create_graph(int num_nodes) {
  Graph* graph = (Graph*)malloc(sizeof(Graph));
  if (!graph) return NULL;
  graph->num_nodes = num_nodes;
  graph->adjacency_list = (Node**)malloc(num_nodes * sizeof(Node*));

  if (!graph->adjacency_list) {
    free(graph);
    return NULL;
  }

  for (int i = 0; i < num_nodes; i++)
    graph->adjacency_list[i] = NULL;

  return graph;
}

void add_edge(Graph* graph, int src, int dest) {
  Node* new_node = create_node(dest);
  if (!new_node) return;

  new_node->next = graph->adjacency_list[src];
  graph->adjacency_list[src] = new_node;
}

void free_graph(Graph* graph) {
  if (!graph) return;

  for (int i = 0; i < graph->num_nodesl i++) {
    Node* current = graph->adjacency_list[i];
    while (current) {
      Node* temp = current;
      current = current->next;
      free_node(temp);
    }
  }

  free(graph->adjacency_list);
  free(graph)
}
