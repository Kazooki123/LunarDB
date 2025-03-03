#ifndef CORE_H
#define CORE_H

#include "graph.h"

/**
 * LunarGraph Core - Core functionality for graph operations in LunarDB
 */

// Graph traversal methods
typedef enum {
    TRAVERSE_DFS,  // Depth-first search
    TRAVERSE_BFS   // Breadth-first search
} TraversalMethod;

// Callback for node visitation during traversal
typedef void (*VisitCallback)(int node_id, void* user_data);

/**
 * Traverse the graph starting from a source node
 *
 * @param graph The graph to traverse
 * @param start_node The starting node ID
 * @param method Traversal method (DFS or BFS)
 * @param callback Function to call for each visited node
 * @param user_data User data passed to the callback
 * @return 0 on success, -1 on failure
 */
int traverse_graph(Graph* graph, int start_node, TraversalMethod method,
                   VisitCallback callback, void* user_data);

/**
 * Find shortest path between two nodes using Dijkstra's algorithm
 * (Assumes all edges have weight 1 for this implementation)
 *
 * @param graph The graph
 * @param source Source node ID
 * @param target Target node ID
 * @param path_out Array to store the resulting path (should be pre-allocated)
 * @param path_len_out Pointer to store the path length
 * @return 0 if path found, -1 if no path exists or on error
 */
int find_shortest_path(Graph* graph, int source, int target,
                      int* path_out, int* path_len_out);

/**
 * Check if the graph contains a cycle
 *
 * @param graph The graph to check
 * @return 1 if cycle exists, 0 if no cycle, -1 on error
 */
int has_cycle(Graph* graph);

/**
 * Get all connected components in the graph
 *
 * @param graph The graph
 * @param components Output array of component IDs for each node (pre-allocated)
 * @return Number of connected components, or -1 on error
 */
int get_connected_components(Graph* graph, int* components);

/**
 * Clone a graph
 *
 * @param original The original graph
 * @return A new graph that is a deep clone of the original, or NULL on failure
 */
Graph* clone_graph(Graph* original);

#endif // CORE_H
