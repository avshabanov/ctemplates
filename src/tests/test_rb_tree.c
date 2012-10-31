
#include <utilities/ut/ut.h>
#include <utilities/ut/ut_utility.h>
#include <utilities/alloc.h>

#include <stdlib.h>
#include <string.h>


#define RB_TREE_NS(name)         my_##name
#define RB_TREE_KEY_TYPE         int
#define RB_TREE_COMPARE(a, b)    (a - b)
#define RB_TREE_XMALLOC          xmalloc
#define RB_TREE_XFREE            xfree
#define RB_TREE_ADD_NODE_REQUIRED
#define RB_TREE_FIND_NODE_REQUIRED
#define RB_TREE_REMOVE_NODE_REQUIRED
#include <templates/rb_tree.h>

static void test_simple_rb_tree()
{
    my_tree t;

    UT_BEGIN("test simple rb tree");

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


#define RB_TREE_NS(name)         int_##name
#define RB_TREE_KEY_TYPE         int
#define RB_TREE_COMPARE(a, b)    (a - b)
#define RB_TREE_XMALLOC          xmalloc
#define RB_TREE_XFREE            xfree
#define RB_TREE_IS_VALID_TREE_REQUIRED
#define RB_TREE_ADD_NODE_REQUIRED
#define RB_TREE_FIND_NODE_REQUIRED
#define RB_TREE_REMOVE_NODE_REQUIRED

#define RB_TREE_PRINT_TREE_REQUIRED
#define RB_TREE_PRINT_NODE(stream, node)\
    fprintf(stream, "%d(%s)", node->key, (node->color == RB_TREE_RED ? "R" : "B"))

#define RB_TREE_FOREACH_REQUIRED

#include <templates/rb_tree.h>

#define GT_NS(name)          int_##name
#include "test_generic_tree.h"

static void test_int_rb_tree1()
{
    int_test_generic_tree("int-generic-rb-tree-test-1");
}

static void tst_foreach_cb(void * c, int_node * node)
{
    assert(c == NULL);
    fprintf(stderr, "(%d)%s ", node->key, node->color == RB_TREE_BLACK ? "Blk" : "Red");
}

static void test_int_rb_tree2()
{
    int_tree tree;
    int nodes[9];
    size_t i;

    UT_BEGIN("rb test2");

    ut_init_ascending_naturals(nodes, 9);

    int_init_tree(&tree);

    for (i = 0; i < (sizeof(nodes)/sizeof(nodes[0])); ++i)
    {
        int key = nodes[i];
        int_node * n = int_add_node(&tree, key);
        UT_VERIFY_SILENT((n->key == key) && int_is_valid_tree(&tree));
    }

    int_prn_tree(&tree, "rbtst2 tree");

    int_tree_foreach(&tree, NULL, &tst_foreach_cb);

    int_uninit_tree(&tree);
    UT_END();
}

/*
 * tree of double numbers
 */

#define RB_TREE_NS(name)             dbl_##name
#define RB_TREE_KEY_TYPE             double
#define RB_TREE_REMOVE_NODE_REQUIRED
#define RB_TREE_FIND_NODE_REQUIRED
#define RB_TREE_COMPARE(l, r)        (int)(l - r)
#define RB_TREE_XMALLOC              xmalloc
#define RB_TREE_XFREE                xfree
#define RB_TREE_IS_VALID_TREE_REQUIRED

#include <templates/rb_tree.h>

static void test_dbl_rb_tree1()
{
    dbl_tree tree;
    int i;
    const int max = 500;

    UT_BEGIN("double rb-tree test");

    dbl_init_tree(&tree);

    /* add ordered array */
    for (i = 1; i < max; ++i)
    {
        dbl_node * added_node;
        bool found;
        double key = (double)(i + 1);

        added_node = dbl_add_node_ext(&tree, key, &found);
        UT_VERIFY_SILENT((added_node != NULL) && (added_node->key = key) && !found);
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
 * test tree with values
 */
#define RB_TREE_NS(name)         intv_##name
#define RB_TREE_KEY_TYPE         int
#define RB_TREE_COMPARE(a, b)    (a - b)
#define RB_TREE_XMALLOC          xmalloc
#define RB_TREE_XFREE            xfree
#define RB_TREE_IS_VALID_TREE_REQUIRED
#define RB_TREE_ADD_NODE_REQUIRED
#define RB_TREE_FIND_NODE_REQUIRED
#define RB_TREE_REMOVE_NODE_REQUIRED

struct UserData
{
    char name[16];
    int attribute;
};

#define RB_TREE_USER_DATA_TYPE   struct UserData

#define RB_TREE_PRINT_TREE_REQUIRED
#define RB_TREE_PRINT_NODE(stream, node)\
    fprintf(stream, "%d - %s(%s)", node->key, node->value.name, (node->color == RB_TREE_RED ? "R" : "B"))

#define RB_TREE_FOREACH_REQUIRED

#include <templates/rb_tree.h>

#define GT_NS(name)          intv_##name
#define GT_TEST_ARR_SIZE     (256)

#define GT_CHECK_VALUE_REQUIRED
#define GT_INIT_NODE_VALUE(node)\
    node->value.attribute = 1000 + node->key;\
    sprintf(node->value.name, "%d", node->key + 8);

#define GT_CHECK_NODE_VALUE(node)\
    UT_VERIFY_SILENT(node->value.attribute == (1000 + node->key));\
    {\
        char tst[8];\
        sprintf(tst, "%d", node->key + 8);\
        UT_VERIFY_SILENT(0 == strcmp(tst, node->value.name));\
    }

#include "test_generic_tree.h"

static void test_intv_rb_tree1()
{
    intv_test_generic_tree("int-generic-rb-tree-test-4");
}

static void test_intv_rb_tree2()
{
    intv_tree tree;
    int nodes[9];
    size_t i;

    UT_BEGIN("rb test5");

    ut_init_ascending_naturals(nodes, 9);

    intv_init_tree(&tree);

    for (i = 0; i < (sizeof(nodes)/sizeof(nodes[0])); ++i)
    {
        int key = nodes[i];
        bool found;
        intv_node * n = intv_add_node_ext(&tree, key, &found);
        UT_VERIFY_SILENT((n->key == key) && intv_is_valid_tree(&tree) && !found);

        n->value.attribute = (int)i;
        sprintf(n->value.name, "%d", key + 1000);
    }

    intv_prn_tree(&tree, "rbtst5 before remove");
    intv_remove_node(&tree, 4);

    UT_VERIFY(intv_is_valid_tree(&tree));

    intv_prn_tree(&tree, "rbtst5 tree");

    intv_uninit_tree(&tree);
    UT_END();
}

void test_rb_tree()
{
    test_simple_rb_tree();
    test_int_rb_tree1();
    test_int_rb_tree2();
    test_dbl_rb_tree1();
    test_intv_rb_tree1();
    test_intv_rb_tree2();
}
