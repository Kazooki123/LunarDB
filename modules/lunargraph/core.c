#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "core.h"

typedef struct QueueNode {
    int data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode *front, *rear;
} Queue;


Queue* create_queue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;
    queue->front = queue->rear = NULL;
    return queue;
}

int is_empty(Queue* queue) {
    return queue->front == NULL;
}

void enqueue(Queue* queue, int value) {
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));
    if (!new_node) return;
    new_node->data = value;
    new_node->next = NULL;

    if (is_empty(queue)) {
        queue->front = queue->rear = new_node;
        return;
    }

    queue->rear->next = new_node;
    queue->rear = new_node;
}

int dequeue(Queue* queue) {
    if (is_empty(queue)) return -1;

    QueueNode* temp = queue->front;
    int value = temp->data;

    queue->front = queue->front->next;
    if (queue->front == NULL) queue->rear = NULL;

    free(temp);
    return value;
}

void free_queue(Queue* queue) {
    if (!queue) return;

    while (!is_empty(queue)) {
        dequeue(queue);
    }

    free(queue);
}

static void dfs_recursive(Graph* graph, int node, bool* visited,
                         VisitCallback callback, void* user_data) {
    visited[node] = true;
    callback(node, user_data);

    Node* current = graph->adjacency_list[node];
    while (current) {
        if (!visited[current->id]) {
            dfs_recursive(graph, current->id, visited, callback, user_data);
        }
        current = current->next;
    }
}

static void bfs(Graph* graph, int start_node, bool* visited,
               VisitCallback callback, void* user_data) {
    Queue* queue = create_queue();
    if (!queue) return;

    visited[start_node] = true;
    enqueue(queue, start_node);

    while (!is_empty(queue)) {
        int current_node = dequeue(queue);
        callback(current_node, user_data);

        Node* current = graph->adjacency_list[current_node];
        while (current) {
            if (!visited[current->id]) {
                visited[current->id] = true;
                enqueue(queue, current->id);
            }
            current = current->next;
        }
    }

    free_queue(queue);
}

int traverse_graph(Graph* graph, int start_node, TraversalMethod method,
                   VisitCallback callback, void* user_data) {
    if (!graph || !callback || start_node < 0 || start_node >= graph->num_nodes) {
        return -1;
    }

    bool* visited = (bool*)calloc(graph->num_nodes, sizeof(bool));
    if (!visited) return -1;

    if (method == TRAVERSE_DFS) {
        dfs_recursive(graph, start_node, visited, callback, user_data);
    } else {
        bfs(graph, start_node, visited, callback, user_data);
    }

    free(visited);
    return 0;
}

int find_shortest_path(Graph* graph, int source, int target,
                      int* path_out, int* path_len_out) {
    if (!graph || !path_out || !path_len_out ||
        source < 0 || source >= graph->num_nodes ||
        target < 0 || target >= graph->num_nodes) {
        return -1;
    }

    bool* visited = (bool*)calloc(graph->num_nodes, sizeof(bool));
    int* parent = (int*)malloc(graph->num_nodes * sizeof(int));
    if (!visited || !parent) {
        free(visited);
        free(parent);
        return -1;
    }

    for (int i = 0; i < graph->num_nodes; i++) {
        parent[i] = -1;
    }

    Queue* queue = create_queue();
    if (!queue) {
        free(visited);
        free(parent);
        return -1;
    }

    visited[source] = true;
    enqueue(queue, source);

    bool found_path = false;
    while (!is_empty(queue) && !found_path) {
        int current = dequeue(queue);

        if (current == target) {
            found_path = true;
            break;
        }

        Node* neighbor = graph->adjacency_list[current];
        while (neighbor) {
            if (!visited[neighbor->id]) {
                visited[neighbor->id] = true;
                parent[neighbor->id] = current;
                enqueue(queue, neighbor->id);
            }
            neighbor = neighbor->next;
        }
    }

    free_queue(queue);

    if (found_path) {
        int length = 0;
        int current = target;
        while (current != source) {
            length++;
            current = parent[current];
        }

        *path_len_out = length + 1;

        current = target;
        for (int i = length; i > 0; i--) {
            path_out[i] = current;
            current = parent[current];
        }
        path_out[0] = source;

        free(visited);
        free(parent);
        return 0;
    }

    free(visited);
    free(parent);
    return -1;
}

static bool has_cycle_util(Graph* graph, int node, bool* visited, bool* in_stack) {
    if (!visited[node]) {
        visited[node] = true;
        in_stack[node] = true;

        Node* current = graph->adjacency_list[node];
        while (current) {
            if (!visited[current->id] && has_cycle_util(graph, current->id, visited, in_stack)) {
                return true;
            } else if (in_stack[current->id]) {
                return true;
            }
            current = current->next;
        }
    }

    in_stack[node] = false;
    return false;
}

int has_cycle(Graph* graph) {
    if (!graph) return -1;

    bool* visited = (bool*)calloc(graph->num_nodes, sizeof(bool));
    bool* in_stack = (bool*)calloc(graph->num_nodes, sizeof(bool));

    if (!visited || !in_stack) {
        free(visited);
        free(in_stack);
        return -1;
    }

    for (int i = 0; i < graph->num_nodes; i++) {
        if (!visited[i] && has_cycle_util(graph, i, visited, in_stack)) {
            free(visited);
            free(in_stack);
            return 1;
        }
    }

    free(visited);
    free(in_stack);
    return 0;
}

static void dfs_component(Graph* graph, int node, bool* visited, int component_id, int* components) {
    visited[node] = true;
    components[node] = component_id;

    Node* current = graph->adjacency_list[node];
    while (current) {
        if (!visited[current->id]) {
            dfs_component(graph, current->id, visited, component_id, components);
        }
        current = current->next;
    }
}

int get_connected_components(Graph* graph, int* components) {
    if (!graph || !components) return -1;

    bool* visited = (bool*)calloc(graph->num_nodes, sizeof(bool));
    if (!visited) return -1;

    int component_count = 0;

    for (int i = 0; i < graph->num_nodes; i++) {
        if (!visited[i]) {
            dfs_component(graph, i, visited, component_count, components);
            component_count++;
        }
    }

    free(visited);
    return component_count;
}

Graph* clone_graph(Graph* original) {
    if (!original) return NULL;

    Graph* clone = create_graph(original->num_nodes);
    if (!clone) return NULL;

    for (int i = 0; i < original->num_nodes; i++) {
        Node* current = original->adjacency_list[i];

        int* edges = NULL;
        int edge_count = 0;

        while (current) {
            edge_count++;
            current = current->next;
        }

        if (edge_count > 0) {
            edges = (int*)malloc(edge_count * sizeof(int));
            if (!edges) {
                free_graph(clone);
                return NULL;
            }

            current = original->adjacency_list[i];
            for (int j = edge_count - 1; j >= 0; j--) {
                edges[j] = current->id;
                current = current->next;
            }

            for (int j = 0; j < edge_count; j++) {
                add_edge(clone, i, edges[j]);
            }

            free(edges);
        }
    }

    return clone;
}
