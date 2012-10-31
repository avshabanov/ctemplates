/*
 * foreach test for either AVL and RB trees
 *
 * A. Shabanov, 2009
 */

#ifndef TREE_NS
#error Namespace specifier is not found
#endif

#ifndef TREE_ELEMENT_TYPE
#error Element type is not found
#endif

#include <string.h>
#include <assert.h>

/*
 * test avl tree 2
 */

struct TREE_NS(ForeachContext)
{
    TREE_ELEMENT_TYPE * arr;
    size_t current_index;
    size_t total;

    bool check_prev_elem;

    TREE_ELEMENT_TYPE prev_elem;

    /*
     * error: previous element is not less than the current one
     */
    bool prev_elem_is_not_less;

    /*
     * current index exceeds total value
     */
    bool current_index_overflow;
};

static void TREE_NS(test_foreach_callback)(void * context, TREE_NS(node) * node)
{
    struct TREE_NS(ForeachContext) * c = context;

    if (c->check_prev_elem && (node->key <= c->prev_elem))
    {
        c->prev_elem_is_not_less = true;
    }

    c->check_prev_elem = true;
    c->prev_elem = node->key;
    
    if (c->current_index < c->total)
    {
        c->arr[c->current_index++] = node->key;
    }
    else
    {
        c->current_index_overflow = true;
    }
}

static void TREE_NS(test_foreach)(TREE_NS(tree) * tree,
                                TREE_ELEMENT_TYPE * orig_arr, size_t total,
                                const char * test_name
                                )
{
    struct TREE_NS(ForeachContext) c;

    UT_BEGIN(test_name);

    UT_VERIFY_CRITICAL((tree != NULL) && (orig_arr != NULL));

    memset(&c, 0, sizeof(c));

    size_t arr_size = total * sizeof(TREE_ELEMENT_TYPE);
    c.arr = xmalloc(arr_size);
    memset(c.arr, 0, arr_size);
    c.total = total;
    
    TREE_NS(tree_foreach)(tree, &c, &TREE_NS(test_foreach_callback));

    if (c.prev_elem_is_not_less || c.current_index_overflow)
    {
        UT_VERIFY_SILENT(!c.prev_elem_is_not_less);
        UT_VERIFY_SILENT(!c.current_index_overflow);
    }
    else
    {
        size_t i;
        size_t j;
        UT_VERIFY_SILENT(c.current_index == total);
        
        for (i = 0; i < total; ++i)
        {
            TREE_ELEMENT_TYPE e = orig_arr[i];
            bool element_exists = false;

            /* uses linear search to find the given key */
            for (j = 0; j < total; ++j)
            {
                if (c.arr[j] == e)
                {
                    element_exists = true;
                    break;
                }
            }

            UT_VERIFY_SILENT(element_exists);
        }
    }

    xfree(c.arr);

    UT_END();
}

#undef TREE_NS
#undef TREE_ELEMENT_TYPE
