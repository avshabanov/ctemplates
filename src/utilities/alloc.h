#pragma once

#include <stddef.h>

void * xmalloc(size_t size);

void * xrealloc(void * p, size_t size);

void xfree(void * p);
