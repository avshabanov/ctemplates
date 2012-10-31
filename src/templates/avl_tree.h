
/*
 * template recursive-based implementation of the AVL tree.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  AVL_TREE_KEY_TYPE - defines key element type
 *  AVL_TREE_COMPARE - defines key comparison macro
 *  AVL_TREE_XMALLOC - defines memory allocation function, that never returns 0
 *  AVL_TREE_XFREE - defines memory releasing function
 *
 * optional macros:
 *  AVL_TREE_NS - namespace/mask macro
 *  AVL_TREE_FIND_NODE_REQUIRED - specifies, that find_node function is required
 *  AVL_TREE_ADD_NODE_REQUIRED - specifies, that add_node function is required
 *  AVL_TREE_REMOVE_NODE_REQUIRED - specifies, that remove_node function is required
 *  AVL_TREE_PRINT_TREE_REQUIRED - specifies, that print_tree function is required
 *  AVL_TREE_IS_VALID_TREE_REQUIRED - specifies, that is_tree_valid function is required
 *  AVL_TREE_PRINT_NODE - print macro, that shall be defined to make AVL_TREE_PRINT_TREE_REQUIRED work
 *  AVL_TREE_INITIAL_CHUNK_SIZE - defines initial chunk size in bytes for internally used nodes allocator
 *  AVL_TREE_USER_DATA_TYPE - defines user data to be added to the node
 *  AVL_TREE_COUNT_REQUIRED - specifies that nodes count shall be provided
 *  AVL_TREE_FOREACH_REQUIRED - specifies, that tree_foreach function is required
 *  AVL_TREE_CLEAR_REQUIRED - specifies that tree_clear function is required
 *  AVL_TREE_ASSERT - specifies assertion macro
 *
 * unmasked types/functions:
 *  node                            - represents single node with key and value fields
 *  tree                            - represents tree structure with internal nodes allocator
 *  internal_search_context
 *  init_tree                       - initializes given tree
 *  uninit_tree                     - uninitializes given tree, must be done on the once initialized tree
 *  find_node                       - finds the node with the key specified
 *  add_node_ext                    - adds node to the tree with the key specified, returns added/existing node with "found" boolean specifier
 *  add_node                        - adds node to the tree with the key specified, returns added node or the existing one
 *  remove_node                     - removes the node specified
 *  is_valid_tree                   - checks whether the tree structure and contents is sane
 *  print_tree                      - prints tree contents to the FILE stream
 *  tree_foreach                    - enumerates all the node in the ascending order
 *  tree_clear                      - clear tree contents
 *  internal_alloc_node
 *  internal_rotate_left
 *  internal_rotate_right
 *  internal_balance_left
 *  internal_balance_right
 *  internal_replace_left_subtree
 *  internal_remove_node_indirect
 *  internal_add_node_indirect
 *  internal_get_tree_height
 *  internal_check_balance
 *
 * Alexander Shabanov, 2008-2009
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

/*
 sample usage:

    // define type names
    #define AVL_TREE_KEY_TYPE            int

    // if user data needed in addition to the key
    // the following definitions may be used
    //#define AVL_TREE_USER_DATA_TYPE      char

    #define AVL_TREE_PRINT_TREE_REQUIRED
    #define AVL_TREE_PRINT_NODE(stream, node)\
        fprintf(stream, "%d(%d)", node->key, node->balance)

    #define AVL_TREE_IS_VALID_TREE_REQUIRED

    #include <templates/avl_tree.h>
 */

#include <stddef.h>
#include <stdbool.h>


/*
 * name specifier definition
 */
#ifndef AVL_TREE_NS
#define AVL_TREE_NS(name) name
#endif

/*
 * typename's related
 */

#ifndef AVL_TREE_KEY_TYPE
#error AVL_TREE_KEY_TYPE is not defined
#endif

/*
 * imported functions
 */

#ifndef AVL_TREE_COMPARE
#error AVL_TREE_COMPARE is not defined
#endif

#ifndef AVL_TREE_XMALLOC
#error AVL_TREE_XMALLOC for avl tree has not been defined
#endif

#ifndef AVL_TREE_XFREE
#error AVL_TREE_XFREE for avl tree has not been defined
#endif

/*
 * utility
 */

#ifndef AVL_TREE_ASSERT
#include <assert.h>
#define AVL_TREE_ASSERT(condition)   assert(condition)
#endif

/*
 * tree node structure
 */
typedef struct AVL_TREE_NS(node)
{
    AVL_TREE_KEY_TYPE           key;

#ifdef AVL_TREE_USER_DATA_TYPE
    /*
     * user-defined data
     */
    AVL_TREE_USER_DATA_TYPE     value;
#endif

    int                         balance;
    struct AVL_TREE_NS(node) *  left;
    struct AVL_TREE_NS(node) *  right;
} AVL_TREE_NS(node);

/*
 * instantiate allocator
 */
#define AVL_FIXED_ALLOC_NS(name)        AVL_TREE_NS(internal_allocator_##name)

#define FIXED_ALLOC_NS(name)            AVL_FIXED_ALLOC_NS(name)
#define FIXED_ALLOC_ELEMENT_TYPE        AVL_TREE_NS(node)
#define FIXED_ALLOC_XMALLOC             AVL_TREE_XMALLOC
#define FIXED_ALLOC_XFREE               AVL_TREE_XFREE
#define FIXED_ALLOC_ASSERT              AVL_TREE_ASSERT

#ifdef AVL_TREE_INITIAL_CHUNK_SIZE
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE  AVL_TREE_INITIAL_CHUNK_SIZE
#endif

#ifdef AVL_TREE_REMOVE_NODE_REQUIRED
#define FIXED_ALLOC_FREE_FUNCTION_REQUIRED
#endif

#ifdef AVL_TREE_IS_VALID_TREE_REQUIRED
#define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
#endif

#include "fixed_alloc.h"

/*
 * tree context definition
 */
typedef struct AVL_TREE_NS(tree)
{
    /*
     * allocator context that is used to store nodes
     */
    AVL_FIXED_ALLOC_NS(allocator)   allocator;

    /*
     * tree's root element
     */
    AVL_TREE_NS(node) *             root;

    /*
     * sentinel element, that is used to specify null leaf nodes
     */
    AVL_TREE_NS(node)               sentinel;

#ifdef AVL_TREE_COUNT_REQUIRED
    /*
     * total tree nodes count
     */
    size_t                          count;
#endif
} AVL_TREE_NS(tree);


static void
AVL_TREE_NS(init_tree)(AVL_TREE_NS(tree) * tree)
{
    AVL_TREE_ASSERT(tree != NULL);
    AVL_FIXED_ALLOC_NS(init_allocator)(&tree->allocator);
    tree->root = &tree->sentinel;

#ifdef AVL_TREE_COUNT_REQUIRED
    tree->count = 0;
#endif
}

static void
AVL_TREE_NS(uninit_tree)(AVL_TREE_NS(tree) * tree)
{
    AVL_TREE_ASSERT(tree != NULL);
    AVL_FIXED_ALLOC_NS(uninit_allocator)(&tree->allocator);
}

#ifdef AVL_TREE_FIND_NODE_REQUIRED

static AVL_TREE_NS(node) *
AVL_TREE_NS(find_node)(AVL_TREE_NS(tree) * tree, AVL_TREE_KEY_TYPE key)
{
    AVL_TREE_NS(node) * node = tree->root;
    AVL_TREE_NS(node) * sentinel = &tree->sentinel;

    sentinel->key = key;

    for (;;)
    {
        int cmp = AVL_TREE_COMPARE(node->key, key);

        if (cmp < 0)
        {
            node = node->right;
        }
        else if (cmp > 0)
        {
            node = node->left;
        }
        else
        {
            break;
        }
    }

    return (node == sentinel ? 0 : node);
}

#endif // AVL_TREE_FIND_NODE_REQUIRED

static AVL_TREE_NS(node) *
AVL_TREE_NS(internal_alloc_node)(AVL_TREE_NS(tree) * tree, AVL_TREE_KEY_TYPE key)
{
    // allocate new node on place
    AVL_TREE_NS(node) * node = AVL_FIXED_ALLOC_NS(alloc_elem)(&tree->allocator);

    // create an empty node
    node->key = key;
    node->left = node->right = &tree->sentinel;
    node->balance = 0;

    return node;
}

typedef struct AVL_TREE_NS(internal_search_context)
{
    AVL_TREE_NS(tree) *     tree;
    AVL_TREE_KEY_TYPE       key;
    bool                    height_changed;

    /*
     * indicates whether or not element with the key given has been found
     * this member shall be preinitialized before the remove operation
     */
    bool                    found;
} AVL_TREE_NS(internal_search_context);



static AVL_TREE_NS(node) *
AVL_TREE_NS(internal_rotate_left)(AVL_TREE_NS(node) * node)
{
    AVL_TREE_NS(node) * l = node->left;
    if (-1 == l->balance)
    {
        /* ordinary LL rotation */
        node->left = l->right;
        l->right = node;
        node->balance = 0;

        node = l;
    }
    else
    {
        /* double LR rotation */
        AVL_TREE_NS(node) * lr = l->right;
        l->right = lr->left;
        lr->left = l;
        node->left = lr->right;
        lr->right = node;

        node->balance = (-1 == lr->balance ? 1 : 0);
        l->balance = (1 == lr->balance ? -1 : 0);

        node = lr;
    }

    node->balance = 0;
    return node;
}

static AVL_TREE_NS(node) *
AVL_TREE_NS(internal_rotate_right)(AVL_TREE_NS(node) * node)
{
    AVL_TREE_NS(node) * r = node->right;
    if (1 == r->balance)
    {
        /* ordinary RR rotation */
        node->right = r->left;
        r->left = node;
        node->balance = 0;

        node = r;
    }
    else
    {
        /* double RL rotation */
        AVL_TREE_NS(node) * rl = r->left;
        r->left = rl->right;
        rl->right = r;
        node->right = rl->left;
        rl->left = node;

        node->balance = (1 == rl->balance ? -1 : 0);
        r->balance = (-1 == rl->balance ? 1 : 0);

        node = rl;
    }

    node->balance = 0;
    return node;
}

static AVL_TREE_NS(node) *
AVL_TREE_NS(internal_add_node_indirect)(AVL_TREE_NS(internal_search_context) * i, AVL_TREE_NS(node) ** pp_node)
{
    AVL_TREE_NS(node) * sentinel = &i->tree->sentinel;
    AVL_TREE_NS(node) * node = *pp_node;
    AVL_TREE_NS(node) * result;

    if (sentinel == node)
    {
        result = AVL_TREE_NS(internal_alloc_node)(i->tree, i->key);
        *pp_node = result;
        i->height_changed = true;
        i->found = false;
#ifdef AVL_TREE_COUNT_REQUIRED
        ++i->tree->count;
#endif
    }
    else
    {
        int cmp = AVL_TREE_COMPARE(node->key, i->key);

        if (cmp > 0)
        {
            result = AVL_TREE_NS(internal_add_node_indirect)(i, &node->left);

            /* left subtree has been grown? */
            if (i->height_changed)
            {
                if (1 == node->balance)
                {
                    node->balance = 0;
                    i->height_changed = false;
                }
                else if (0 == node->balance)
                {
                    node->balance = -1;
                }
                else
                {
                    /* balancing is required */
                    *pp_node = AVL_TREE_NS(internal_rotate_left)(node);
                    i->height_changed = false;
                } /* node->balance == -1? */
            } /* height changed? */
        } /* node->key > key? */
        else if( cmp < 0 )
        {
            result = AVL_TREE_NS(internal_add_node_indirect)(i, &node->right);

            /* right subtree has been grown? */
            if (i->height_changed)
            {
                if (-1 == node->balance)
                {
                    node->balance = 0;
                    i->height_changed = false;
                }
                else if (0 == node->balance)
                {
                    node->balance = 1;
                }
                else
                {
                    /* balancing is required */
                    *pp_node = AVL_TREE_NS(internal_rotate_right)(node);
                    i->height_changed = false;
                } /* node->balance == 1? */
            } /* height changed? */
        } /* node->key < key? */
        else
        {
            /* key matches the existing node, no insertion needed */
            result = node;
            i->found = true;
        } /* node->key == key? */
    } /* non-sentinel node? */

    return result;
}

static inline AVL_TREE_NS(node) *
AVL_TREE_NS(add_node_ext)(AVL_TREE_NS(tree) * tree, AVL_TREE_KEY_TYPE key, bool * found)
{
    AVL_TREE_NS(internal_search_context) i;
    AVL_TREE_NS(node) *          result;

    i.tree = tree;
    i.key = key;
    i.height_changed = false;

    result = AVL_TREE_NS(internal_add_node_indirect)(&i, &tree->root);
    *found = i.found;

    return result;
}

#ifdef AVL_TREE_ADD_NODE_REQUIRED

static AVL_TREE_NS(node) *
AVL_TREE_NS(add_node)(AVL_TREE_NS(tree) * tree, AVL_TREE_KEY_TYPE key)
{
    AVL_TREE_NS(internal_search_context) i;

    i.tree = tree;
    i.key = key;
    i.height_changed = false;

    return AVL_TREE_NS(internal_add_node_indirect)(&i, &tree->root);
}

#endif // AVL_TREE_ADD_NODE_REQUIRED



#ifdef AVL_TREE_REMOVE_NODE_REQUIRED

static void
AVL_TREE_NS(internal_balance_left)(AVL_TREE_NS(internal_search_context) * i, AVL_TREE_NS(node) ** pp_node )
{
    AVL_TREE_NS(node) * node = *pp_node;
    if (-1 == node->balance)
    {
        node->balance = 0;
    }
    else if (0 == node->balance)
    {
        node->balance = 1;
        i->height_changed = false;
    }
    else
    {
        /* balancing is required */
        AVL_TREE_NS(node) * r = node->right;
        if (r->balance >= 0)
        {
            /* ordinary RR rotation */
            node->right = r->left;
            r->left = node;

            if (r->balance == 0)
            {
                node->balance = 1;
                r->balance = -1;
                i->height_changed = false;
            }
            else
            {
                node->balance = r->balance = 0;
            }

            node = r;
        }
        else
        {
            /* double RL rotation */
            AVL_TREE_NS(node) * rl = r->left;
            r->left = rl->right;
            rl->right = r;
            node->right = rl->left;
            rl->left = node;

            node->balance = (1 == rl->balance ? -1 : 0);
            r->balance = (-1 == rl->balance ? 1 : 0);

            rl->balance = 0;

            node = rl;
        }

        *pp_node = node;
    } /* node->balance == -1? */
}

static void
AVL_TREE_NS(internal_balance_right)(AVL_TREE_NS(internal_search_context) * i, AVL_TREE_NS(node) ** pp_node)
{
    AVL_TREE_NS(node) * node = *pp_node;
    if (1 == node->balance)
    {
        node->balance = 0;
    }
    else if (0 == node->balance)
    {
        node->balance = -1;
        i->height_changed = false;
    }
    else
    {
        /* balancing is required */
        AVL_TREE_NS(node) * l = node->left;
        if (l->balance <= 0)
        {
            /* ordinary LL rotation */
            node->left = l->right;
            l->right = node;

            if (l->balance == 0)
            {
                node->balance = -1;
                l->balance = 1;
                i->height_changed = false;
            }
            else
            {
                node->balance = l->balance = 0;
            }

            node = l;
        }
        else
        {
            /* double LR rotation */
            AVL_TREE_NS(node) * lr = l->right;
            l->right = lr->left;
            lr->left = l;
            node->left = lr->right;
            lr->right = node;

            node->balance = (-1 == lr->balance ? 1 : 0);
            l->balance = (1 == lr->balance ? -1 : 0);

            lr->balance = 0;

            node = lr;
        }

        *pp_node = node;
    } /* node->balance == -1? */
}


static AVL_TREE_NS(node) *
AVL_TREE_NS(internal_replace_left_subtree)(
    AVL_TREE_NS(internal_search_context) * i,
    AVL_TREE_NS(node) ** pp_node
    )
{
    AVL_TREE_NS(node) * sentinel = &i->tree->sentinel;
    AVL_TREE_NS(node) * node = *pp_node;
    AVL_TREE_NS(node) * result;

    if (sentinel != node->right)
    {
        result = AVL_TREE_NS(internal_replace_left_subtree)(i, &node->right);
        if (i->height_changed)
        {
            AVL_TREE_NS(internal_balance_right)(i, pp_node);
        }
    }
    else
    {
        result = node;
        *pp_node = node->left;
        i->height_changed = true;
    }

    return result;
}

static void
AVL_TREE_NS(internal_remove_node_indirect)(AVL_TREE_NS(internal_search_context) * i, AVL_TREE_NS(node) ** pp_node)
{
    AVL_TREE_NS(node) * sentinel = &i->tree->sentinel;
    AVL_TREE_NS(node) * node = *pp_node;

    if (sentinel != node)
    {
        int cmp = AVL_TREE_COMPARE(node->key, i->key);

        if (cmp > 0)
        {
            AVL_TREE_NS(internal_remove_node_indirect)(i, &node->left);

            /* left subtree has been decreased? */
            if (i->height_changed)
            {
                AVL_TREE_NS(internal_balance_left)(i, pp_node);
            } /* height changed? */
        } /* node->key > key? */
        else if (cmp < 0)
        {
            AVL_TREE_NS(internal_remove_node_indirect)(i, &node->right);

            /* right subtree has been decreased? */
            if (i->height_changed)
            {
                AVL_TREE_NS(internal_balance_right)(i, pp_node);
            } /* height changed? */
        } /* node->key < key? */
        else
        {
            i->found = true;

            /* key found - remove it */
            if (sentinel == node->right)
            {
                *pp_node = node->left;
                i->height_changed = true;
            }
            else if (sentinel == node->left)
            {
                *pp_node = node->right;
                i->height_changed = true;
            }
            else
            {
                AVL_TREE_NS(node) * ls = AVL_TREE_NS(internal_replace_left_subtree)(i, &node->left);

                ls->left = node->left;
                ls->right = node->right;
                ls->balance = node->balance;

                *pp_node = ls;

                if (i->height_changed)
                {
                    AVL_TREE_NS(internal_balance_left)(i, pp_node);
                }
            }

            /* deallocate node */
            AVL_FIXED_ALLOC_NS(free_elem)(&i->tree->allocator, node);
#ifdef AVL_TREE_COUNT_REQUIRED
            --i->tree->count;
#endif
        } /* node->key == key? */
    } /* non-sentinel node? */
}

static bool
AVL_TREE_NS(remove_node)(AVL_TREE_NS(tree) * tree, AVL_TREE_KEY_TYPE key)
{
    AVL_TREE_NS(internal_search_context) i;

    i.tree = tree;
    i.key = key;
    i.height_changed = false;
    i.found = false;

    AVL_TREE_NS(internal_remove_node_indirect)(&i, &tree->root);

    return i.found;
}

#endif // AVL_TREE_REMOVE_NODE_REQUIRED


#ifdef AVL_TREE_PRINT_TREE_REQUIRED

/*
 * the following macro -
 * AVL_TREE_PRINT_NODE(stream, node)
 * supposed to be defined
 */
#ifndef AVL_TREE_PRINT_NODE
#error AVL_TREE_NS(print_tree) expects AVL_TREE_PRINT_NODE to be defined
#endif

static void
AVL_TREE_NS(print_tree)(FILE * os, AVL_TREE_NS(tree) * tree, AVL_TREE_NS(node) * node, int indentation)
{
    if (&tree->sentinel != node)
    {
        int i;

        AVL_TREE_NS(print_tree)(os, tree, node->left, indentation + 1);

        for (i = 0; i < indentation; ++ i)
        {
            fputc(' ', os);
        }

        AVL_TREE_PRINT_NODE(os, node);
        fputc('\n', os);

        AVL_TREE_NS(print_tree)(os, tree, node->right, indentation + 1);
    }
}
#endif // AVL_TREE_PRINT_TREE_REQUIRED


#ifdef AVL_TREE_IS_VALID_TREE_REQUIRED

static int
AVL_TREE_NS(internal_get_tree_height)(AVL_TREE_NS(tree) * t, AVL_TREE_NS(node) * node)
{
    AVL_TREE_NS(node) * sentinel = &t->sentinel;

    if (sentinel != node)
    {
        int lh = AVL_TREE_NS(internal_get_tree_height)(t, node->left);
        int rh = AVL_TREE_NS(internal_get_tree_height)(t, node->right);

        return 1 + (lh > rh ? lh : rh);
    }

    return 0;
}

static bool
AVL_TREE_NS(internal_check_balance)(AVL_TREE_NS(tree) * t, AVL_TREE_NS(node) * node, size_t * calculated_nodes_count)
{
    if (&t->sentinel != node)
    {
        int lh = AVL_TREE_NS(internal_get_tree_height)(t, node->left);
        int rh = AVL_TREE_NS(internal_get_tree_height)(t, node->right);
        int balance = rh - lh;

        ++(*calculated_nodes_count);

        if (node->balance != balance)
        {
            return false;
        }

        if (!AVL_TREE_NS(internal_check_balance)(t, node->left, calculated_nodes_count))
        {
            return false;
        }

        if (!AVL_TREE_NS(internal_check_balance)(t, node->right, calculated_nodes_count))
        {
            return false;
        }
    }

    return true;
}

static bool
AVL_TREE_NS(is_valid_tree)(AVL_TREE_NS(tree) * t)
{
    size_t calculated_nodes_count = 0;
    if (AVL_TREE_NS(internal_check_balance)(t, t->root, &calculated_nodes_count))
    {
        size_t used_nodes;
        size_t allocated_nodes;

        AVL_FIXED_ALLOC_NS(get_allocator_status)(&t->allocator,
            &used_nodes, &allocated_nodes);

        if (used_nodes != calculated_nodes_count)
        {
            return false;
        }
    }
    return true;
}

#endif // AVL_TREE_IS_VALID_TREE_REQUIRED

#ifdef AVL_TREE_FOREACH_REQUIRED

typedef struct
{
    AVL_TREE_NS(tree) * tree;

    void * context;

    void (* foreach_callback)(void * context, AVL_TREE_NS(node) * node);
} AVL_TREE_NS(InternalForeachContext);

static void AVL_TREE_NS(internal_tree_foreach)(
    AVL_TREE_NS(InternalForeachContext) * c,
    AVL_TREE_NS(node) * node)
{
    if (node != &c->tree->sentinel)
    {
        AVL_TREE_NS(internal_tree_foreach)(c, node->left);

        c->foreach_callback(c->context, node);

        AVL_TREE_NS(internal_tree_foreach)(c, node->right);
    }
}

/*
 * iterates through all the tree nodes
 */
static void AVL_TREE_NS(tree_foreach)(AVL_TREE_NS(tree) * tree,
                                         void * context,
                                         void (* foreach_callback)(void * context, AVL_TREE_NS(node) * node))
{
    AVL_TREE_NS(InternalForeachContext) c;
    c.tree = tree;
    c.context = context;
    c.foreach_callback = foreach_callback;

    AVL_TREE_NS(internal_tree_foreach)(&c, tree->root);
}

#endif // AVL_TREE_FOREACH_REQUIRED

#ifdef AVL_TREE_CLEAR_REQUIRED

static void AVL_TREE_NS(tree_clear)(AVL_TREE_NS(tree) * tree)
{
    AVL_TREE_NS(uninit_tree)(tree);
    AVL_TREE_NS(init_tree)(tree);
}

#endif

/*
 * undefine fixed alloc namespace specifier
 */
#undef AVL_FIXED_ALLOC_NS

/*
 * undefine user macros
 */
#undef AVL_TREE_KEY_TYPE
#undef AVL_TREE_XMALLOC
#undef AVL_TREE_XREALLOC
#undef AVL_TREE_XFREE
#undef AVL_TREE_COMPARE
#undef AVL_TREE_NS
#undef AVL_TREE_FIND_NODE_REQUIRED
#undef AVL_TREE_ADD_NODE_REQUIRED
#undef AVL_TREE_REMOVE_NODE_REQUIRED
#undef AVL_TREE_PRINT_TREE_REQUIRED
#undef AVL_TREE_IS_VALID_TREE_REQUIRED
#undef AVL_TREE_PRINT_NODE
#undef AVL_TREE_INITIAL_CHUNK_SIZE
#undef AVL_TREE_USER_DATA_TYPE
#undef AVL_TREE_COUNT_REQUIRED
#undef AVL_TREE_FOREACH_REQUIRED
#undef AVL_TREE_CLEAR_REQUIRED
