#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

int lunar_vector_create(const char *db_key);

int lunar_vector_add(const char *db_key, const char *vector_id, const float *data, int data_len);

int lunar_vector_search(const char *db_key,
                        const float *query,
                        int query_len,
                        int limit,
                        char **results,
                        float *scores);

int lunar_vector_set_ttl(const char *db_key, const char *vector_id, int ttl_seconds);

void lunar_vector_cleanup(void);
