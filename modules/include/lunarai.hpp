#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

enum class LunarAIStatusCode {
  Success = 0,
  InvalidInput = 1,
  ModelError = 2,
  StorageError = 3,
  EmbeddingError = 4,
  RAGError = 5,
  UnknownError = 99,
};

struct LunarAI;

struct LunarAIHandle {
  LunarAI *_0;
};

extern "C" {

LunarAIHandle lunar_ai_create();

void lunar_ai_destroy(LunarAIHandle handle);

const char *lunar_ai_version();

LunarAIStatusCode lunar_ai_generate_embedding(LunarAIHandle handle,
                                              const char *text,
                                              float *embedding_out,
                                              int *embedding_size);

LunarAIStatusCode lunar_ai_compute_similarity(LunarAIHandle handle,
                                              const float *embedding1,
                                              const float *embedding2,
                                              int size,
                                              float *similarity_out);

void lunar_ai_free_string(char *ptr);

} // extern "C"
