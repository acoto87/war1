#pragma once

#include <stdint.h>

void* __xmalloc(size_t size, char *file, int32_t line);
void* __xcalloc(size_t count, size_t size, char *file, int32_t line);
void* __xrealloc(void *ptr, size_t size, char *file, int32_t line);

#define xmalloc(size) __xmalloc(size, __FILE__, __LINE__)
#define xcalloc(count, size) __xcalloc(count, size, __FILE__, __LINE__)
#define xrealloc(ptr, size) __xrealloc(ptr, size, __FILE__, __LINE__)
