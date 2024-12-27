#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct Stream Stream;

struct Stream *stream_new(const char *name);

char *stream_add_message(struct Stream *stream, const char *payload);

void stream_free(struct Stream *stream);

void string_free(char *s);
