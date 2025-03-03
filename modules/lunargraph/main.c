#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "core.h"

#define MAX_PATH_LENGTH 100
#define MAX_BUFFER 1024

typedef struct {
    char* key;
    char* value;
} GraphData;

typedef struct {
    Graph* schema_graph;
    Graph* data_graph;
    GraphData** node_data;
    int data_capacity;
    int data_size;
} LunarGraph;

LunarGraph* lunar_graph_init(int schema_nodes, int data_nodes) {
    LunarGraph* lg = (LunarGraph*)malloc(sizeof(LunarGraph));
    if (!lg) return NULL;

    lg->schema_graph = create_graph(schema_nodes);
    lg->data_graph = create_graph(data_nodes);

    if (!lg->schema_graph || !lg->data_graph) {
        if (lg->schema_graph) free_graph(lg->schema_graph);
        if (lg->data_graph) free_graph(lg->data_graph);
        free(lg);
        return NULL;
    }

    lg->data_capacity = data_nodes;
    lg->data_size = 0;
    lg->node_data = (GraphData**)malloc(data_nodes * sizeof(GraphData*));

    if (!lg->node_data) {
        free_graph(lg->schema_graph);
        free_graph(lg->data_graph);
        free(lg);
        return NULL;
    }

    for (int i = 0; i < data_nodes; i++) {
        lg->node_data[i] = NULL;
    }

    return lg;
}

int lunar_graph_add_schema_relation(LunarGraph* lg, int src_type, int dest_type) {
    if (!lg || src_type < 0 || src_type >= lg->schema_graph->num_nodes ||
        dest_type < 0 || dest_type >= lg->schema_graph->num_nodes) {
        return -1;
    }

    add_edge(lg->schema_graph, src_type, dest_type);
    return 0;
}

int lunar_graph_add_data_node(LunarGraph* lg, int type_id, const char* key, const char* value) {
    if (!lg || type_id < 0 || type_id >= lg->schema_graph->num_nodes || !key || !value) {
        return -1;
    }

    if (lg->data_size >= lg->data_capacity) {
        int new_capacity = lg->data_capacity * 2;
        GraphData** new_data = (GraphData**)realloc(lg->node_data, new_capacity * sizeof(GraphData*));
        if (!new_data) return -1;

        Graph* new_graph = create_graph(new_capacity);
        if (!new_graph) {
            return -1;
        }

        for (int i = 0; i < lg->data_size; i++) {
            Node* current = lg->data_graph->adjacency_list[i];
            while (current) {
                add_edge(new_graph, i, current->id);
                current = current->next;
            }
        }

        for (int i = lg->data_capacity; i < new_capacity; i++) {
            new_data[i] = NULL;
        }

        free_graph(lg->data_graph);
        lg->data_graph = new_graph;
        lg->node_data = new_data;
        lg->data_capacity = new_capacity;
    }

    GraphData* data = (GraphData*)malloc(sizeof(GraphData));
    if (!data) return -1;

    data->key = strdup(key);
    data->value = strdup(value);

    if (!data->key || !data->value) {
        if (data->key) free(data->key);
        if (data->value) free(data->value);
        free(data);
        return -1;
    }

    lg->node_data[lg->data_size] = data;
    int node_id = lg->data_size++;

    return node_id;
}

int lunar_graph_connect_data(LunarGraph* lg, int src_id, int dest_id) {
    if (!lg || src_id < 0 || src_id >= lg->data_size ||
        dest_id < 0 || dest_id >= lg->data_size) {
        return -1;
    }

    add_edge(lg->data_graph, src_id, dest_id);
    return 0;
}

char* lunar_graph_get_value(LunarGraph* lg, int node_id) {
    if (!lg || node_id < 0 || node_id >= lg->data_size || !lg->node_data[node_id]) {
        return NULL;
    }

    return lg->node_data[node_id]->value;
}

void collect_related_values(int node_id, void* user_data) {
    LunarGraph* lg = ((void**)user_data)[0];
    char* buffer = ((void**)user_data)[1];
    int* buffer_size = ((void**)user_data)[2];

    char* value = lunar_graph_get_value(lg, node_id);
    if (value) {
        int len = strlen(value);
        if (*buffer_size + len + 2 < MAX_BUFFER) {
            if (*buffer_size > 0) {
                strcat(buffer, ",");
                (*buffer_size)++;
            }
            strcat(buffer, value);
            *buffer_size += len;
        }
    }
}

char* lunar_graph_query_related(LunarGraph* lg, int start_node) {
    if (!lg || start_node < 0 || start_node >= lg->data_size) {
        return NULL;
    }

    char* result_buffer = (char*)malloc(MAX_BUFFER);
    if (!result_buffer) return NULL;
    result_buffer[0] = '\0';

    int buffer_size = 0;
    void* user_data[3] = {lg, result_buffer, &buffer_size};

    traverse_graph(lg->data_graph, start_node, TRAVERSE_BFS, collect_related_values, user_data);

    return result_buffer;
}

int lunar_graph_path_exists(LunarGraph* lg, int source, int target) {
    if (!lg || source < 0 || source >= lg->data_size ||
        target < 0 || target >= lg->data_size) {
        return -1;
    }

    int path[MAX_PATH_LENGTH];
    int path_len = 0;

    int result = find_shortest_path(lg->data_graph, source, target, path, &path_len);
    return (result == 0) ? 1 : 0;
}

void lunar_graph_free(LunarGraph* lg) {
    if (!lg) return;

    free_graph(lg->schema_graph);
    free_graph(lg->data_graph);

    for (int i = 0; i < lg->data_size; i++) {
        if (lg->node_data[i]) {
            free(lg->node_data[i]->key);
            free(lg->node_data[i]->value);
            free(lg->node_data[i]);
        }
    }

    free(lg->node_data);
    free(lg);
}

int main() {
    LunarGraph* lg = lunar_graph_init(3, 10);

    lunar_graph_add_schema_relation(lg, 0, 1);
    lunar_graph_add_schema_relation(lg, 1, 2);

    int person1 = lunar_graph_add_data_node(lg, 0, "person:1", "{\"name\":\"Alice\",\"age\":30}");
    int person2 = lunar_graph_add_data_node(lg, 0, "person:2", "{\"name\":\"Bob\",\"age\":25}");

    int post1 = lunar_graph_add_data_node(lg, 1, "post:1", "{\"title\":\"Hello World\",\"content\":\"First post\"}");
    int post2 = lunar_graph_add_data_node(lg, 1, "post:2", "{\"title\":\"Graph DB\",\"content\":\"Interesting topic\"}");

    int comment1 = lunar_graph_add_data_node(lg, 2, "comment:1", "{\"text\":\"Great post!\"}");
    int comment2 = lunar_graph_add_data_node(lg, 2, "comment:2", "{\"text\":\"I agree\"}");

    lunar_graph_connect_data(lg, person1, post1);
    lunar_graph_connect_data(lg, person2, post2);
    lunar_graph_connect_data(lg, post1, comment1);
    lunar_graph_connect_data(lg, post1, comment2);
    lunar_graph_connect_data(lg, person2, comment1);

    char* related_to_alice = lunar_graph_query_related(lg, person1);
    printf("Data related to Alice: %s\n", related_to_alice);
    free(related_to_alice);

    int path_exists = lunar_graph_path_exists(lg, person2, comment2);
    printf("Path from Bob to comment 'I agree': %s\n", path_exists ? "Yes" : "No");

    printf("LunarGraph module initialization complete with %d nodes.\n", lg->data_size);

    lunar_graph_free(lg);
    return 0;
}
