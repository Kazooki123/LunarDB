#include <stdio.h>
#include "graph.h"

int main() {
  Graph* graph = create_graph(5);

  add_edge(graph, 0, 1);
  add_edge(graph, 0, 4);
  add_edge(graph, 1, 2);
  add_edge(graph, 1, 3);
  add_edge(graph, 1, 4);

  printf("Graph created with %d nodes.\n", graph->num_nodes);

  free_graph(graph);
  return 0;
}
