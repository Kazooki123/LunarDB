#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum FFIError {
  Success = 0,
  EmbeddingError = 1,
  ModelError = 2,
  RAGError = 3,
  NullPointer = 4,
  InvalidUtf8 = 5,
} FFIError;

typedef struct AIPipeline AIPipeline;

typedef struct Embedding Embedding;

typedef struct Option_RAGSystem Option_RAGSystem;

typedef struct LunarAI {
  struct AIPipeline pipeline;
  struct Option_RAGSystem rag_system;
} LunarAI;

struct LunarAI *lunar_ai_new(const char *model_path);

void lunar_ai_free(struct LunarAI *ptr);

struct Embedding *lunar_ai_generate_embedding(struct LunarAI *ptr,
                                              const char *text,
                                              enum FFIError *error);
