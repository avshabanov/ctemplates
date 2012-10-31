
#include "ut_utility.h"

#include <stdlib.h>

void ut_init_ascending_naturals(int * arr, size_t total)
{
    size_t i;

    for (i = 0; i < total; ++i)
    {
        arr[i] = (int)i + 1;
    }
}

void ut_permutate(int * arr, size_t total, size_t perm_loop_count)
{
    size_t i;
    size_t j;

    for (j = 0; j < perm_loop_count; ++j)
    {
        for (i = 0; i < total; ++i)
        {
            int k = arr[i];
            int p = rand() % (int)total;

            arr[i] = arr[p];
            arr[p] = k;
        }
    }
}
