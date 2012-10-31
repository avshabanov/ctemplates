
#include <utilities/ut/ut.h>
#include <utilities/ut/ut_utility.h>
#include <utilities/alloc.h>

#include <stdlib.h>



#define AVL_TREE_NS(name)         my_##name
#define AVL_TREE_KEY_TYPE         int
#define AVL_TREE_COMPARE(a, b)    (a - b)
#define AVL_TREE_XMALLOC          xmalloc
#define AVL_TREE_XFREE            xfree
#define AVL_TREE_ADD_NODE_REQUIRED
#define AVL_TREE_FIND_NODE_REQUIRED
#define AVL_TREE_REMOVE_NODE_REQUIRED
#include <templates/avl_tree.h>

static void test_simple_avl_tree()
{
    my_tree t;

    UT_BEGIN("test simple avl tree");

    my_init_tree(&t);

    my_add_node(&t, 10);
    my_add_node(&t, 40);
    my_add_node(&t, 30);
    my_add_node(&t, 20);
    my_add_node(&t, 50);

    my_remove_node(&t, 30);

    UT_VERIFY(my_find_node(&t, 10) != 0);
    UT_VERIFY(my_find_node(&t, 20) != 0);
    UT_VERIFY(my_find_node(&t, 40) != 0);
    UT_VERIFY(my_find_node(&t, 50) != 0);
    UT_VERIFY(my_find_node(&t, 30) == 0);

    my_uninit_tree(&t);

    UT_END();
}



// complex tests


#define AVL_TREE_NS(name)            int_##name
#define AVL_TREE_KEY_TYPE            int
#define AVL_TREE_ADD_NODE_REQUIRED
#define AVL_TREE_REMOVE_NODE_REQUIRED
#define AVL_TREE_FIND_NODE_REQUIRED
#define AVL_TREE_USER_DATA_TYPE      const char *
#define AVL_TREE_COMPARE(l, r)       (l - r)
#define AVL_TREE_XMALLOC             xmalloc
#define AVL_TREE_XFREE               xfree

#define AVL_TREE_PRINT_TREE_REQUIRED
#define AVL_TREE_PRINT_NODE(stream, node)\
    fprintf(stream, "%d(%d)", node->key, node->balance)

#define AVL_TREE_IS_VALID_TREE_REQUIRED
#define AVL_TREE_COUNT_REQUIRED
#define AVL_TREE_FOREACH_REQUIRED

#include <templates/avl_tree.h>

#define GT_NS(name)          int_##name
#include "test_generic_tree.h"

static void test_avl_tree1()
{
    int_test_generic_tree("int-generic-avl-tree-test-1");
}


static void test_avl_tree2()
{
    int_tree tree;
    const size_t total = 500;
    size_t i;
    int * arr = xmalloc(sizeof(int) * total);

    UT_BEGIN("avl tree 2");

    int_init_tree(&tree);

    /* fill tree w/random numbers from 1 to total */
    
    /* initialize array */
    ut_init_ascending_naturals(arr, total);

    /* perform 2-cycle permutations */
    ut_permutate(arr, total, 2);
    
    /* fill tree */
    for (i = 0; i < total; ++i)
    {
        bool found;
        int_node * n = int_add_node_ext(&tree, arr[i], &found);
        UT_VERIFY_SILENT((n != NULL) && (n->key == arr[i]) && !found);
    }

    /* integrity test */
    {
        /* test nodes count */
        UT_VERIFY(tree.count == total);

        /* test foreach function */
        int_test_foreach(&tree, arr, total, "test foreach");

        /* test find node */
        for (i = 0; i < total; ++i)
        {
            int_node * n;
            int key = (int)i + 1;

            n = int_find_node(&tree, key);
            UT_VERIFY_SILENT((n != NULL) && (n->key == key));
        }

        UT_VERIFY(int_find_node(&tree, (int)total + 1) == NULL);
        UT_VERIFY(int_is_valid_tree(&tree));
    }

    /* test nodes removal */
    {
        const size_t removalIndex = total / 2;
        size_t nodesLeft = total - removalIndex;

        /* randomize order */
        ut_permutate(arr, total, 2);

        /* remove 1/2 nodes */
        for (i = 0; i < removalIndex; ++i)
        {
            int key = (int)i + 1;
            UT_VERIFY_SILENT(int_remove_node(&tree, key));
            UT_VERIFY_SILENT(int_is_valid_tree(&tree));
        }

        UT_VERIFY(tree.count = nodesLeft);

        /* check that nodes really removed */
        for (i = 0; i < removalIndex; ++i)
        {
            int_node * n;
            int key = (int)i + 1;
            UT_VERIFY_SILENT(!int_remove_node(&tree, key));

            n = int_find_node(&tree, key);
            UT_VERIFY_SILENT(n == NULL);
        }

        /* check that other nodes still exists */
        UT_VERIFY(tree.count = nodesLeft);
    }

    int_uninit_tree(&tree);
    xfree(arr);
    UT_END();
}


/*
 * tree of double numbers
 */

#define AVL_TREE_NS(name)            dbl_##name
#define AVL_TREE_KEY_TYPE            double
#define AVL_TREE_REMOVE_NODE_REQUIRED
#define AVL_TREE_FIND_NODE_REQUIRED
#define AVL_TREE_COMPARE(l, r)       (int)(l - r)
#define AVL_TREE_XMALLOC             xmalloc
#define AVL_TREE_XFREE               xfree
#define AVL_TREE_IS_VALID_TREE_REQUIRED

#include <templates/avl_tree.h>

static void test_dbl_avl_tree()
{
    dbl_tree tree;
    int i;
    const int max = 500;

    UT_BEGIN("double tree test");

    dbl_init_tree(&tree);

    /* add ordered array */
    for (i = 1; i < max; ++i)
    {
        dbl_node * added_node;
        bool found;
        double key = (double)(i + 1);

        added_node = dbl_add_node_ext(&tree, key, &found);
        UT_VERIFY_SILENT((added_node != NULL) && (added_node->key = key) && !found);
        UT_VERIFY_SILENT(NULL != dbl_find_node(&tree, key));
    }

    UT_VERIFY(dbl_is_valid_tree(&tree));

    for (i = 1; i < max; ++i)
    {
        double key = (double)(i + 1);

        UT_VERIFY_SILENT(dbl_remove_node(&tree, key));
        UT_VERIFY_SILENT(dbl_is_valid_tree(&tree));
    }

    /* remove all the nodes away */

    dbl_uninit_tree(&tree);
    UT_END();
}

/*
 * test cases pack
 */
void test_avl_tree()
{
    test_simple_avl_tree();
    test_avl_tree1();
    test_avl_tree2();
    test_dbl_avl_tree();
}

