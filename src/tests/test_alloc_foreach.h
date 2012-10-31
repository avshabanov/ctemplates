
#ifndef NS
#error NS is not defined
#endif

#ifndef ELEMENT_TYPE
#error ELEMENT_TYPE is not defined
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct NS(ForeachContext)
{
    ELEMENT_TYPE * fill_arr;
    size_t current;
    size_t total;
    bool overflow;
};

static void NS(foreach_cb)(void * c,
#ifdef FOREACH_ELEMENT_IS_VOID
                              void * p
#else
                              ELEMENT_TYPE * e
#endif
                              )
{
#ifdef FOREACH_ELEMENT_IS_VOID
    ELEMENT_TYPE * e = p;
#endif
    struct NS(ForeachContext) * fc = (struct NS(ForeachContext) *)c;
    if (fc->current < fc->total)
    {
        fc->fill_arr[fc->current++] = *e;
    }
    else
    {
        fc->overflow = true;
    }
}

static int NS(internal_elements_compare)(const void * lhs, const void * rhs)
{
    const ELEMENT_TYPE l = *((const ELEMENT_TYPE *)lhs);
    const ELEMENT_TYPE r = *((const ELEMENT_TYPE *)rhs);

    return (int)(l - r);
}

static void NS(test_foreach)(const char * test_name, NS(allocator) * allocator, ELEMENT_TYPE * arr, size_t total)
{
    struct NS(ForeachContext) fc;
    const size_t fill_arr_size = sizeof(ELEMENT_TYPE) * total;

    UT_BEGIN(test_name);

    memset(&fc, 0, sizeof(fc));
    fc.fill_arr = xmalloc(fill_arr_size);
    memset(fc.fill_arr, 0, fill_arr_size);
    fc.total = total;

    NS(allocator_foreach)(allocator, &fc, &NS(foreach_cb));

    UT_VERIFY_SILENT(fc.current == total);
    UT_VERIFY_SILENT(!fc.overflow);

    if ((fc.current == total) && !fc.overflow)
    {
        size_t i;

        qsort(fc.fill_arr, fc.total, sizeof(ELEMENT_TYPE), &NS(internal_elements_compare));

        for (i = 0; i < total; ++i)
        {
            ELEMENT_TYPE key = arr[i];
            void * found_elem_p = bsearch(&key, fc.fill_arr, fc.total,
                sizeof(ELEMENT_TYPE), &NS(internal_elements_compare));

            UT_VERIFY_SILENT(found_elem_p != NULL);
        }
    }

    {
        size_t used;
        size_t allocated;

        NS(get_allocator_status)(allocator, &used, &allocated);
        UT_VERIFY_SILENT(used == total);
    }

    xfree(fc.fill_arr);
    UT_END();
}



#undef NS
#undef ELEMENT_TYPE
