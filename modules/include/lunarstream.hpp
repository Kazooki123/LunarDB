#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

struct Stream;

extern "C" {

Stream *stream_new(const char *name);

char *stream_add_message(Stream *stream, const char *payload);

void stream_free(Stream *stream);

void string_free(char *s);

} // extern "C"
