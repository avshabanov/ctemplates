#include <stdio.h>
#include <stdlib.h>

#include "alloc.h"

void * xmalloc(size_t size)
{
    void * result = malloc(size);
    if (result == NULL)
    {
        fprintf(stderr, "error: malloc failed to allocate %ud bytes, now abort\n", (unsigned int)size);
        abort();
    }
    return result;
}

void * xrealloc(void * p, size_t size)
{
    void * result = realloc(p, size);
    if (result == NULL && size > 0)
    {
        fprintf(stderr, "error: realloc failed to reallocate %ud bytes, now abort\n", (unsigned int)size);
        abort();
    }
    return result;
}

void xfree(void * p)
{
    free(p);
}
