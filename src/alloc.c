#include <stdlib.h>
#include <stdio.h>

#include "alloc.h"

void* __xmalloc(size_t size, char *file, int32_t line)
{
    void *ptr = malloc(size);
    if (!ptr)
    {
        perror("xmalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* __xcalloc(size_t count, size_t size, char *file, int32_t line)
{
    void *ptr = calloc(count, size);
    if (!ptr)
    {
        perror("xcalloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}

void* __xrealloc(void *ptr, size_t size, char *file, int32_t line)
{
    ptr = realloc(ptr, size);
    if (!ptr)
    {
        perror("xrealloc failed!");
        if (file)
        {
            fprintf(stderr, "  at file %s (%d)\n", file, line);
        }
        exit(1);
    }

    return ptr;
}
