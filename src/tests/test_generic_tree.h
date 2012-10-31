
/*
 * provides generic test facilities for tree w/integer numbers
 *
 * A. Shabanov
 */

#ifndef GT_NS
#error GT_NS is not defined
#endif

#ifndef GT_TEST_ARR_SIZE
#define GT_TEST_ARR_SIZE     (256)
#endif

#ifdef GT_CHECK_VALUE_REQUIRED

#ifndef GT_INIT_NODE_VALUE
#error GT_INIT_NODE_VALUE is not defined
#endif

#ifndef GT_CHECK_NODE_VALUE
#error GT_CHECK_NODE_VALUE is not defined
#endif

#endif

/* this is for 'foreach' test */
#define TREE_NS                 GT_NS
#define TREE_ELEMENT_TYPE       int
#include "test_tree_foreach.h"

static void GT_NS(prn_tree)(GT_NS(tree) * tree, const char * tree_name)
{
    fputs(tree_name, stderr);
    fputs("\n", stderr);
    GT_NS(print_tree)(stderr, tree, tree->root, 0);
}


static void GT_NS(test_add_remove)(const char * test_name, int * arr, size_t total)
{
    GT_NS(tree) tree;
    size_t i;

    UT_BEGIN(test_name);

    GT_NS(init_tree)(&tree);

    /* fill tree */
    for (i = 0; i < total; ++i)
    {
        int key = arr[i];
        GT_NS(node) * node = GT_NS(add_node)(&tree, key);
        UT_VERIFY_SILENT((node != NULL) && (node->key == arr[i]));
        UT_VERIFY_SILENT(GT_NS(is_valid_tree)(&tree));

#ifdef GT_CHECK_VALUE_REQUIRED
        if (node != NULL)
        {
            GT_INIT_NODE_VALUE(node);
        }
#endif
    }

    /* try to remove nodes in the random order */
    ut_permutate(arr, total, 1);

    /* test foreach function */
    GT_NS(test_foreach)(&tree, arr, total, test_name);

    /* remove nodes */
    for (i = 0; i < total; ++i)
    {
        int key = arr[i];
        //bool dbg_brk = ((i == 0) && (key == 8));
        bool dbg_brk = (key == 1111000022);

        if (dbg_brk)
        {
            GT_NS(prn_tree)(&tree, "before failure");
            fprintf(stderr, "now removing %d\n", key);
            getchar();
        }

#ifdef GT_CHECK_VALUE_REQUIRED
        {
            GT_NS(node) * found_node = GT_NS(find_node)(&tree, key);
            UT_VERIFY_SILENT(found_node != NULL);
            if (found_node != NULL)
            {
                GT_CHECK_NODE_VALUE(found_node);
            }
        }
#else
        UT_VERIFY_SILENT(GT_NS(find_node)(&tree, key) != NULL);
#endif
        UT_VERIFY_SILENT(GT_NS(remove_node)(&tree, key));

        if (dbg_brk)
        {
            GT_NS(prn_tree)(&tree, "after failure");
            getchar();
        }

        UT_VERIFY_SILENT(GT_NS(is_valid_tree)(&tree));
        UT_VERIFY_SILENT(GT_NS(find_node)(&tree, key) == NULL);
    }

    GT_NS(uninit_tree)(&tree);
    UT_END();
}

static void GT_NS(test_generic_tree)(const char * test_name)
{
    int arr[GT_TEST_ARR_SIZE];
    size_t i;
    size_t j;

    /* initialize ascending array w/unique values */
    ut_init_ascending_naturals(arr, GT_TEST_ARR_SIZE);

    GT_NS(test_add_remove)(test_name, arr, GT_TEST_ARR_SIZE);

    ut_permutate(arr, GT_TEST_ARR_SIZE, 1);

    GT_NS(test_add_remove)(test_name, arr, GT_TEST_ARR_SIZE);

    /* ascending-descending sequence */
    j = GT_TEST_ARR_SIZE / 2;
    for (i = 0; i < j; ++i)
    {
        arr[i] = (int)i + 1;
    }
    for (i = j; i < GT_TEST_ARR_SIZE; ++i)
    {
        arr[i] = (int)i + 1;
    }

    GT_NS(test_add_remove)(test_name, arr, GT_TEST_ARR_SIZE);
}

#undef GT_NS
#undef GT_TEST_ARR_SIZE
#undef GT_CHECK_VALUE_REQUIRED
#undef GT_INIT_NODE_VALUE
#undef GT_CHECK_NODE_VALUE
