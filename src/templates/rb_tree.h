
/*
 * template non-recursive implementation of the red-black tree.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  RB_TREE_KEY_TYPE - defines key element type
 *  RB_TREE_COMPARE - defines key comparison macro
 *  RB_TREE_XMALLOC - defines memory allocation function, that will never return 0
 *  RB_TREE_XFREE - defines memory disposal function
 *
 * optional macros:
 *  RB_TREE_NS - namespace macro
 *  RB_TREE_FIND_NODE_REQUIRED - specifies, that find_node function is required
 *  RB_TREE_ADD_NODE_REQUIRED - specifies, that add_node function is required
 *  RB_TREE_REMOVE_NODE_REQUIRED - specifies, that remove_node function is required
 *  RB_TREE_PRINT_TREE_REQUIRED - specifies, that print_tree function is required
 *  RB_TREE_PRINT_NODE - print macro, that shall be defined to make print_tree function work
 *  RB_TREE_IS_VALID_TREE_REQUIRED - specifies, that is_tree_valid function is required
 *  RB_TREE_INITIAL_CHUNK_SIZE - defines initial chunk size in bytes for internally used nodes allocator
 *  RB_TREE_USER_DATA_TYPE - defines user data to be added to the node
 *  RB_TREE_COUNT_REQUIRED - specifies that nodes count shall be provided
 *  RB_TREE_FOREACH_REQUIRED - specifies, that tree_foreach function is required
 *  RB_TREE_CLEAR_REQUIRED - specifies that tree_clear function is required
 *  RB_TREE_ASSERT - specifies assertion macro
 *
 * unmasked types/functions:
 *  node                            node structure with the key and value fields
 *  tree                            tree structure with internal nodes allocator
 *  init_tree                       initializes tree
 *  uninit_tree                     uninitializes tree, must be done on the previously initialized tree
 *  find_node                       finds the node with the key specified
 *  add_node_ext                    adds node to the tree with the key specified, returns added/existing node with "found" boolean specifier
 *  add_node                        adds node to the tree with the key specified, returns added node or the existing one
 *  remove_node                     removes the node specified
 *  is_valid_tree                   checks whether the tree structure and contents is sane
 *  print_tree                      prints tree contents to the FILE stream
 *  tree_foreach                    enumerates all the node in the ascending order
 *  tree_clear                      clear tree contents
 *
 *  internal_check_context
 *  internal_alloc_node
 *  internal_get_holder
 *  internal_rotate_left_pp
 *  internal_rotate_right_pp
 *  internal_rotate_left
 *  internal_rotate_right
 *  internal_adjust_tree
 *  internal_replace_nodes
 *  internal_fixup_tree
 *  internal_recursive_check
 *
 * Alexander Shabanov, 2009
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

#include <stddef.h>
#include <stdbool.h>

/*
 * name specifier definition
 */
#ifndef RB_TREE_NS
#define RB_TREE_NS(name) name
#endif

/*
 * typename's related
 */

#ifndef RB_TREE_KEY_TYPE
#error RB_TREE_KEY_TYPE is not defined
#endif

/*
 * imported functions
 */

#ifndef RB_TREE_COMPARE
#error RB_TREE_COMPARE is not defined
#endif

#ifndef RB_TREE_XMALLOC
#error RB_TREE_XMALLOC is not been defined
#endif

#ifndef RB_TREE_XFREE
#error RB_TREE_XFREE is not been defined
#endif

/*
 * utility
 */

#ifndef RB_TREE_ASSERT
#include <assert.h>
#define RB_TREE_ASSERT(condition)   assert(condition)
#endif

/*
 * define rb tree colors
 */
#ifndef RB_TREE_COLORS_DEFINED
#define RB_TREE_COLORS_DEFINED

/*
 * represents color of the red-black tree node
 */
enum
{
    RB_TREE_RED = 0,
    RB_TREE_BLACK = 1,
};

#endif // RB_TREE_COLORS_DEFINED

/*
 * node of the red-black tree
 */
typedef struct RB_TREE_NS(node)
{
    /*
     * element itself
     */
    RB_TREE_KEY_TYPE                key;

#ifdef RB_TREE_USER_DATA_TYPE
    RB_TREE_USER_DATA_TYPE          value;
#endif

    /*
     * red or black, black if head
     */
    char                            color;

    /*
     * left subtree, or smallest element if head
     */
    struct RB_TREE_NS(node) *       left;

    /*
     * right subtree, or largest element if head
     */
    struct RB_TREE_NS(node) *       right;

    /*
     * parent, or root of tree if head
     */
    struct RB_TREE_NS(node) *       parent;
} RB_TREE_NS(node);

/*
 * instantiate allocator
 */
#define RB_TREE_FIXED_ALLOC_NS(name) RB_TREE_NS(internal_allocator_##name)

#define FIXED_ALLOC_NS(name)         RB_TREE_FIXED_ALLOC_NS(name)
#define FIXED_ALLOC_ELEMENT_TYPE     RB_TREE_NS(node)
#define FIXED_ALLOC_XMALLOC          RB_TREE_XMALLOC
#define FIXED_ALLOC_XFREE            RB_TREE_XFREE
#define FIXED_ALLOC_ASSERT           RB_TREE_ASSERT

#ifdef RB_TREE_INITIAL_CHUNK_SIZE
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE   RB_TREE_INITIAL_CHUNK_SIZE
#endif

#ifdef RB_TREE_REMOVE_NODE_REQUIRED
#define FIXED_ALLOC_FREE_FUNCTION_REQUIRED
#endif

#ifdef RB_TREE_IS_VALID_TREE_REQUIRED
#define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
#endif

#include "fixed_alloc.h"

/*
 * red-black tree structure
 */
typedef struct RB_TREE_NS(tree)
{
    /*
     * root element of the tree
     */
    RB_TREE_NS(node) *      root;

    /*
     * dummy node of the tree that represents tree leafs
     */
    RB_TREE_NS(node)        leaf;

    /*
     * nodes allocator
     */
    RB_TREE_FIXED_ALLOC_NS(allocator) allocator;

#ifdef RB_TREE_COUNT_REQUIRED
    /*
     * total nodes count in the tree
     */
    size_t                  count;
#endif
} RB_TREE_NS(tree);


/*
 * initializes tree
 */
static void RB_TREE_NS(init_tree)(RB_TREE_NS(tree) * tree)
{
    RB_TREE_NS(node) * leaf;

    RB_TREE_ASSERT(tree != NULL);
    
    leaf = &tree->leaf;
    leaf->color = RB_TREE_BLACK;
    leaf->left = leaf->right = leaf;
    leaf->parent = NULL;

    tree->root = leaf;

    RB_TREE_FIXED_ALLOC_NS(init_allocator)(&tree->allocator);

#ifdef RB_TREE_COUNT_REQUIRED
    tree->count = 0;
#endif
}

/*
 * uninitializes tree
 */
static void RB_TREE_NS(uninit_tree)(RB_TREE_NS(tree) * tree)
{
    RB_TREE_ASSERT(tree != NULL);
    RB_TREE_FIXED_ALLOC_NS(uninit_allocator)(&tree->allocator);
}


#if defined(RB_TREE_FIND_NODE_REQUIRED) || defined(RB_TREE_REMOVE_NODE_REQUIRED)

/*
 * finds node that has the key given or NULL
 */
static RB_TREE_NS(node) *
RB_TREE_NS(find_node)(RB_TREE_NS(tree) * tree, RB_TREE_KEY_TYPE key)
{
    RB_TREE_NS(node) * node = tree->root;
    RB_TREE_NS(node) * leaf = &tree->leaf;

    leaf->key = key;

    for (;;)
    {
        int cmp = RB_TREE_COMPARE(node->key, key);

        if (cmp > 0)
        {
            node = node->left;
        }
        else if (cmp < 0)
        {
            node = node->right;
        }
        else
        {
            break;
        }
    }

    return (node == leaf ? NULL : node);
}

#endif // RB_TREE_FIND_NODE_REQUIRED || RB_TREE_REMOVE_NODE_REQUIRED


/*
 * allocates one another node, initializes it with the red color and
 * leaf nodes as it's left and right
 */
static RB_TREE_NS(node) *
RB_TREE_NS(internal_alloc_node)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * parent, RB_TREE_KEY_TYPE key)
{
    RB_TREE_NS(node) * node = RB_TREE_FIXED_ALLOC_NS(alloc_elem)(&tree->allocator);
    node->color = RB_TREE_RED;
    node->key = key;
    node->left = node->right = &tree->leaf;
    node->parent = parent;

#ifdef RB_TREE_COUNT_REQUIRED
    ++tree->count;
#endif

    return node;
}

/*
 * retrieves pointer to the holder that contains the given node
 */
static RB_TREE_NS(node) **
RB_TREE_NS(internal_get_holder)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node)
{
    if (node->parent != NULL)
    {
        if (node->parent->left == node)
        {
            return &node->parent->left;
        }

        return &node->parent->right;
    }

    return &tree->root;
}

/*
 * rotates node (pp_dest points to) to the left
 */
static void RB_TREE_NS(internal_rotate_left_pp)(RB_TREE_NS(node) ** pp_dest)
{
    RB_TREE_NS(node) * dest = *pp_dest;
    RB_TREE_NS(node) * child = dest->right;

    *pp_dest = child;
    child->parent = dest->parent;
    dest->parent = child;
    dest->right = child->left;
    dest->right->parent = dest;
    child->left = dest;
}

static void RB_TREE_NS(internal_rotate_left)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node)
{
    RB_TREE_NS(internal_rotate_left_pp)(RB_TREE_NS(internal_get_holder)(tree, node));
}

/*
 * rotates node (pp_dest points to) to the right
 */
static void RB_TREE_NS(internal_rotate_right_pp)(RB_TREE_NS(node) ** pp_dest)
{
    RB_TREE_NS(node) * dest = *pp_dest;
    RB_TREE_NS(node) * child = dest->left;

    *pp_dest = child;
    child->parent = dest->parent;
    dest->parent = child;
    dest->left = child->right;
    dest->left->parent = dest;
    child->right = dest;
}

static void RB_TREE_NS(internal_rotate_right)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node)
{
    RB_TREE_NS(internal_rotate_right_pp)(RB_TREE_NS(internal_get_holder)(tree, node));
}

/*
 * adjusts tree structure after having inserted node into the tree.
 * it is assumed, that node is red
 */
static void
RB_TREE_NS(internal_adjust_tree)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node)
{
    /* adjust tree structure, keeping in mind that inserted node has RED color */
    RB_TREE_ASSERT(node->color == RB_TREE_RED);

    for (;;)
    {
        RB_TREE_NS(node) * grandparent;

        /* root is always black, so set black color manually */
        if (node->parent == NULL)
        {
            node->color = RB_TREE_BLACK;
            break;
        }
        
        /* it is known, that parent != NULL at this step,
         * if parent is black then there is no need to adjust any parameters */
        if (node->parent->color == RB_TREE_BLACK)
        {
            break;
        }

        /* it is known, that parent is red at this step */
        grandparent = node->parent->parent;
        RB_TREE_ASSERT(grandparent != NULL); /* grandparent can't be null at this step due to rb tree nature */

        /* this is the case when the node have an uncle and this uncle is red */
        {
            RB_TREE_NS(node) * uncle = (node->parent == grandparent->left ? grandparent->right : grandparent->left);
            if ((uncle != NULL) && (uncle->color == RB_TREE_RED))
            {
                /* repaint parent and uncle in black and repaint grandparent to red, continue with grandparent */
                node->parent->color = uncle->color = RB_TREE_BLACK;
                grandparent->color = RB_TREE_RED;
                node = grandparent;
                continue;
            }
        }

        /* at this step it is known, that:
         * node is red, node's parent is red, node has no uncle or uncle is black, node has black grandparent.
         */

        if ((node == node->parent->right) && (node->parent == grandparent->left))
        {
            /*     grandparent[BLACK]                 grandparent[BLACK]
             *      /              \                  /               \
             *   parent[RED]        4    =>       node[RED]            4
             *   /          \                     /       \
             *  1         node[RED]          parent[RED]  3
             *            /      \           /         \         
             *           2        3         1           2
             */

            RB_TREE_NS(internal_rotate_left_pp)(&grandparent->left);

            node = node->left; /* or former parent */
        }
        else if ((node == node->parent->left) && (node->parent == grandparent->right))
        {
            /*     grandparent[BLACK]         grandparent[BLACK]
             *      /              \          /                \
             *     1      parent[RED]   =>   1                node[RED]
             *            /         \                         /       \
             *    node[RED]          4                       2      parent[RED]
             *    /       \                                         /        \
             *   2         3                                       3         4
             */

            RB_TREE_NS(internal_rotate_right_pp)(&grandparent->right);

            node = node->right; /* or former parent */
        }
        
        /* at this step the final rotation is performed
         * the structure of tree is as follows (or similar):
         *
         *              grandparent[BLACK]            parent[BLACK]
         *              /               \             /          \
         *          parent[RED]          4  =>     node[RED]   grandparent[RED]
         *          /       \                      /     \     /             \
         *      node[RED]    3                    1      2    3               4
         *     /      \
         *    1        2
         */

        node->parent->color = RB_TREE_BLACK;
        grandparent->color = RB_TREE_RED;

        if ((node == node->parent->left) && (node->parent  == grandparent->left))
        {
            RB_TREE_NS(internal_rotate_right)(tree, grandparent);
        }
        else
        {
            RB_TREE_ASSERT((node == node->parent->right) && (node->parent  == grandparent->right));
            RB_TREE_NS(internal_rotate_left)(tree, grandparent);
        }

        break;
    }
}

/*
 * adds node to the tree or returns an existing one.
 * found indicates whether such a node already exists or not.
 */
static RB_TREE_NS(node) *
RB_TREE_NS(add_node_ext)(RB_TREE_NS(tree) * tree, RB_TREE_KEY_TYPE key, bool * found)
{
    const RB_TREE_NS(node) * leaf = &tree->leaf;
    RB_TREE_NS(node) ** dest_node = &tree->root;
    RB_TREE_NS(node) * node;
    RB_TREE_NS(node) * prev_node = NULL;

    /* find place to insert or an existing node */
    for (;;)
    {
        node = *dest_node;

        if (node == leaf)
        {
            /* new node */
            node = RB_TREE_NS(internal_alloc_node)(tree, prev_node, key);
            *dest_node = node;
            *found = false;
            RB_TREE_NS(internal_adjust_tree)(tree, node);
            break;
        }
        else
        {
            int cmp = RB_TREE_COMPARE(node->key, key);
            if (cmp > 0)
            {
                /* insert to the left */
                dest_node = &node->left;
            }
            else if (cmp < 0)
            {
                /* insert to the right */
                dest_node = &node->right;
            }
            else
            {
                /* such a node already exists */
                *found = true;
                break;
            }

            prev_node = node;
        }
    }

    return node;
}


#ifdef RB_TREE_ADD_NODE_REQUIRED

static RB_TREE_NS(node) *
RB_TREE_NS(add_node)(RB_TREE_NS(tree) * tree, RB_TREE_KEY_TYPE key)
{
    bool found;
    return RB_TREE_NS(add_node_ext)(tree, key, &found);
}

#endif // RB_TREE_ADD_NODE_REQUIRED


#ifdef RB_TREE_REMOVE_NODE_REQUIRED

/*
 * replaces dst node with src
 */
static void
RB_TREE_NS(internal_replace_nodes)(RB_TREE_NS(tree) * tree,
                                      RB_TREE_NS(node) * src, RB_TREE_NS(node) * dst)
{
    RB_TREE_NS(node) ** src_holder = RB_TREE_NS(internal_get_holder)(tree, src);
    
    *src_holder = dst;
    dst->parent = src->parent;
}


static void
RB_TREE_NS(internal_fixup_tree)(RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node)
{
    RB_TREE_NS(node) * leaf = &tree->leaf;

    RB_TREE_ASSERT((node->left == leaf) || (node->right == leaf));

    /* we have node with at least one child */
    {
        RB_TREE_NS(node) * child = ((node->left == leaf) ? node->right : node->left);
        
        RB_TREE_NS(internal_replace_nodes)(tree, node, child);

        /* deleting red node does not violate any rule */
        if (node->color == RB_TREE_RED)
        {
            return;
        }

        /* if single child is red it can be just recolored to black */
        if (child->color == RB_TREE_RED)
        {
            child->color = RB_TREE_BLACK;
            return;
        }

        node = child;
    }

    /* fixup loop */
    for (;;)
    {
        RB_TREE_NS(node) * sibling;
        RB_TREE_NS(node) * parent = node->parent;

        /* given node shall be of the black color */
        RB_TREE_ASSERT(node->color == RB_TREE_BLACK);
        
        /* case 1: discontinue fixup if node is root */
        if (parent == NULL)
        {
            break;
        }

        /* case 2: sibling is red (it is known, that node is black at this point)
         *
         *         parent[BLACK]                    parent[RED]
         *         /           \            =>      /           \
         *      node[BLACK]  sibling[RED]        node[BLACK]  sibling[BLACK]
         */
        sibling = ((parent->left == node) ? parent->right : parent->left);
        if (sibling->color == RB_TREE_RED)
        {
            parent->color = RB_TREE_RED; /* obviously parent was black */
            sibling->color = RB_TREE_BLACK;

            if (node == parent->left)
            {
                /*      parent[BLACK]                   sibling[BLACK]
                 *      /           \           =>      /
                 * node[BLACK]   sibling[RED]       parent[RED]
                 *                                    /
                 *                                  node[BLACK]
                 */
                RB_TREE_NS(internal_rotate_left)(tree, parent);
                sibling = parent->right;
            }
            else
            {
                RB_TREE_NS(internal_rotate_right)(tree, parent);
                sibling = parent->left;
            }
        }

        /* it is known, that sibling is black at this point */
        if ((sibling->left->color == RB_TREE_BLACK) &&
            (sibling->right->color == RB_TREE_BLACK))
        {
            if (parent->color == RB_TREE_BLACK)
            {
                /* case 3:
                 *
                 *      parent[BLACK]
                 *      /           \
                 *  node[BLACK]   sibling[BLACK] => sibling[RED]
                 *                /            \
                 *           sl[BLACK]      sr[BLACK]
                 */
                sibling->color = RB_TREE_RED;
                node = parent;
                continue;
            }
            else
            {
                /* case 4: parent node is red, sibling and it's childs are black
                 *
                 *      parent[RED]                     parent[BLACK]
                 *      /           \                          \
                 *  node[BLACK]   sibling[BLACK] =>    ...   sibling[RED]
                 *                /            \             /          \
                 *           sl[BLACK]      sr[BLACK]    sl[BLACK]  sr[BLACK]
                 *
                 */
                sibling->color = RB_TREE_RED;
                parent->color = RB_TREE_BLACK;
                break;
            }
        }

        /* case 5: it is known that sibling is black, but at least one of it's child is red */
        if ((node == parent->left) &&
            (sibling->left->color == RB_TREE_RED) &&
            (sibling->right->color == RB_TREE_BLACK))
        {
            /*          sibling[BLACK]           sl[BLACK]
             *          /            \                   \
             *      sl[RED]         sr[BLACK] =>        sibling[RED]
             *                                             \
             *                                            sr[BLACK]
             */
            sibling->color = RB_TREE_RED;
            sibling->left->color = RB_TREE_BLACK;
            RB_TREE_NS(internal_rotate_right)(tree, sibling);

            /* sibling will change after rotation */
            sibling = sibling->parent;
        }
        else if ((node == parent->right) &&
            (sibling->right->color == RB_TREE_RED) &&
            (sibling->left->color == RB_TREE_BLACK))
        {
            sibling->color = RB_TREE_RED;
            sibling->right->color = RB_TREE_BLACK;
            RB_TREE_NS(internal_rotate_left)(tree, sibling);

            /* sibling will change after rotation */
            sibling = sibling->parent;
        }

        /* case 6: sibling is black, node is black,
         *         either left or right sibling's child is red, parent's color does not matter
         */

        RB_TREE_ASSERT((sibling->color == RB_TREE_BLACK) && (node->color == RB_TREE_BLACK));

        sibling->color = parent->color;
        parent->color = RB_TREE_BLACK;

        if (node == parent->left)
        {
            RB_TREE_ASSERT(sibling->right->color == RB_TREE_RED);

            sibling->right->color = RB_TREE_BLACK;
            RB_TREE_NS(internal_rotate_left)(tree, parent);
        }
        else
        {
            RB_TREE_ASSERT((sibling->left->color == RB_TREE_RED) && (node == parent->right));

            sibling->left->color = RB_TREE_BLACK;
            RB_TREE_NS(internal_rotate_right)(tree, parent);
        }

        break;
    }
}

/*
 * removes node with the key specified from the tree
 */
static bool
RB_TREE_NS(remove_node)(RB_TREE_NS(tree) * tree, RB_TREE_KEY_TYPE key)
{
    RB_TREE_NS(node) * node;

    node = RB_TREE_NS(find_node)(tree, key);
    if (node != NULL)
    {
        RB_TREE_NS(node) * leaf = &tree->leaf;

        /* special case for nodes w/two childs */
        if ((node->left != leaf) && (node->right != leaf))
        {
            /* find replace candidate as most left element in the right subtree */
            RB_TREE_NS(node) * rc = node->right;

            while (rc->left != leaf)
            {
                rc = rc->left;
            }

            /* swap nodes */
#ifdef RB_TREE_USER_DATA_TYPE
            /* replace nodes */
            {
                RB_TREE_NS(node) ** pp_node_holder = RB_TREE_NS(internal_get_holder)(tree, node);
                char rc_color = rc->color;

                if (node->right == rc)
                {
                    *pp_node_holder = rc;
                    rc->parent = node->parent;
                    rc->left = node->left;
                    rc->color = node->color;
                    rc->left->parent = rc;

                    node->parent = rc;
                    node->right = rc->right;
                    node->right->parent = node;
                    node->left = leaf;
                    node->color = rc_color;

                    rc->right = node;
                }
                else
                {
                    RB_TREE_NS(node) ** pp_rc_holder = RB_TREE_NS(internal_get_holder)(tree, rc);
                    RB_TREE_NS(node) * rc_right = rc->right;
                    RB_TREE_NS(node) * rc_parent = rc->parent;

                    *pp_node_holder = rc;
                    rc->parent = node->parent;
                    rc->left = node->left;
                    rc->right = node->right;
                    rc->color = node->color;
                    rc->left->parent = rc->right->parent = rc;

                    *pp_rc_holder = node;
                    node->parent = rc_parent;
                    node->left = leaf;
                    node->right = rc_right;
                    node->right->parent = node;
                    node->color = rc_color;
                }
            }
#else
            /* just move keys */
            {
                RB_TREE_KEY_TYPE key = node->key;
                node->key = rc->key;
                rc->key = key;
            }

            node = rc;
#endif
        }

        /* fix up tree */
        RB_TREE_NS(internal_fixup_tree)(tree, node);

        /* free element itself */
        RB_TREE_FIXED_ALLOC_NS(free_elem)(&tree->allocator, node);

#ifdef RB_TREE_COUNT_REQUIRED
        --tree->count;
#endif

        return true;
    }
    
    return false;
}

#endif // RB_TREE_REMOVE_NODE_REQUIRED


#ifdef RB_TREE_IS_VALID_TREE_REQUIRED

struct RB_TREE_NS(internal_check_context)
{
    RB_TREE_NS(tree) *      tree;
    int                     already_calculated_path;
    size_t                  calculated_nodes_count;
};

#ifndef RB_TREE_ERR_CODES_DEFINED
#define RB_TREE_ERR_CODES_DEFINED

enum
{
    RB_TREE_ERR_INVALID_COLOR = -1,          /* node has invalid color */
    RB_TREE_ERR_LEAF_ISNT_BLACK = -2,        /* leaf node is not black */
    RB_TREE_ERR_RED_CHILD_ISNT_BLACK = -3,   /* red node's child is not black */
    RB_TREE_ERR_INCONSISTENT_PATH = -4,      /* at least two paths from root to the descendant leaves are different */
    RB_TREE_ERR_INVALID_PARENT_NODE = -5,    /* node->parent does not point to the real parent node */
    RB_TREE_ERR_INCONSISTENT_COMPARE = -6,   /* compare function is not sane */
    RB_TREE_ERR_INCONSISTENT_KEY = -7,       /* node key does not satisfy binary tree constraint */
    RB_TREE_ERR_DANGLING_NODE_FOUND = -8,    /* allocated nodes count is not equals to the nodes in tree */
};

#endif // RB_TREE_ERR_CODES_DEFINED

static int
RB_TREE_NS(internal_recursive_check)(
    struct RB_TREE_NS(internal_check_context) * c,
    RB_TREE_NS(node) * node,
    RB_TREE_NS(node) * prev_node,
    int calculated_path
    )
{
    RB_TREE_NS(tree) * tree = c->tree;
    const RB_TREE_NS(node) * leaf = &tree->leaf;
    
    if (node == leaf)
    {
#if 0
        /* violation of this rule does not affect the entire rb-tree concept, we just assume leaves black,
         * no matter what the leaf->color attribute is.
         */
        /* checking rule: 2. Every leaf (NULL) is black. */
        if (leaf->color != RB_TREE_BLACK)
        {
            return RB_TREE_ERR_LEAF_ISNT_BLACK;
        }
#endif

        if (c->already_calculated_path < 0)
        {
            c->already_calculated_path = calculated_path;
        }
        else if (c->already_calculated_path != calculated_path)
        {
            /* checking rule 4. Every simple path from a node to a descendant leaf contains the same number of black nodes.  */
            return RB_TREE_ERR_INCONSISTENT_PATH;
        }
    }
    else
    {
        int ret;
        RB_TREE_KEY_TYPE key = node->key;
        char prev_node_color = (node->parent == NULL ? RB_TREE_RED : node->parent->color);
        int cmp = RB_TREE_COMPARE(key, node->key);

        /* generic constraint: key should be equal to itself */
        if (cmp != 0)
        {
            return RB_TREE_ERR_INCONSISTENT_COMPARE;
        }

        if (node->parent != prev_node)
        {
            return RB_TREE_ERR_INVALID_PARENT_NODE;
        }

        ++c->calculated_nodes_count;

        /* checking rule 3. If a node is red, then both its children are black. */
        if (node->color == RB_TREE_RED)
        {
            if (prev_node_color == RB_TREE_RED)
            {
                return RB_TREE_ERR_RED_CHILD_ISNT_BLACK;
            }
        }
        else if (node->color == RB_TREE_BLACK)
        {
            /* advance simple path of black nodes */
            calculated_path = calculated_path + 1;
        }
        else
        {
            /* checking rule 1. Every node is either red or black. */
            return RB_TREE_ERR_INVALID_COLOR;
        }

        /* recursively check left node */
        ret = RB_TREE_NS(internal_recursive_check)(c, node->left, node, calculated_path);
        if (ret < 0)
        {
            return ret;
        }

        /* recursively check right node */
        ret = RB_TREE_NS(internal_recursive_check)(c, node->right, node, calculated_path);
        if (ret < 0)
        {
            return ret;
        }

        /* check keys */
        if (node->left != leaf)
        {
            cmp = RB_TREE_COMPARE(node->key, node->left->key);
            if (cmp <= 0)
            {
                return RB_TREE_ERR_INCONSISTENT_KEY;
            }
        }

        if (node->right != leaf)
        {
            cmp = RB_TREE_COMPARE(node->key, node->right->key);
            if (cmp >= 0)
            {
                return RB_TREE_ERR_INCONSISTENT_KEY;
            }
        }
    }

    return 0;
}

/*
 * performs validation check on the given red-black tree,
 * the following rules are checked:
 *  1. Every node is either red or black.
 *  2. Every leaf (NULL) is black.
 *  3. If a node is red, then both its children are black.
 *  4. Every simple path from a node to a descendant leaf contains the same number of black nodes. 
 */
static bool
RB_TREE_NS(is_valid_tree)(RB_TREE_NS(tree) * tree)
{
    int ret;
    struct RB_TREE_NS(internal_check_context) c;
    c.tree = tree;
    c.already_calculated_path = -1;
    c.calculated_nodes_count = 0;

    ret = RB_TREE_NS(internal_recursive_check)(&c, tree->root, NULL, 0);

    /* ensure that allocated nodes count equals to nodes in tree */
    if (ret == 0)
    {
        size_t used_nodes;
        size_t allocated_nodes;

        RB_TREE_FIXED_ALLOC_NS(get_allocator_status)(&tree->allocator, &used_nodes, &allocated_nodes);
        if (used_nodes != c.calculated_nodes_count)
        {
            ret = RB_TREE_ERR_DANGLING_NODE_FOUND;
        }
    }

    return (ret == 0);
}

#endif // RB_TREE_IS_VALID_TREE_REQUIRED


#ifdef RB_TREE_PRINT_TREE_REQUIRED

/*
 * the following macro -
 * RB_TREE_PRINT_NODE(stream, node)
 * supposed to be defined
 */
#ifndef RB_TREE_PRINT_NODE
#error RB_TREE_NS(print_tree) expects RB_TREE_PRINT_NODE to be defined
#endif

static void
RB_TREE_NS(print_tree)(FILE * os, RB_TREE_NS(tree) * tree, RB_TREE_NS(node) * node, int indentation)
{
    if (&tree->leaf != node)
    {
        int i;
        
        RB_TREE_NS(print_tree)(os, tree, node->left, indentation + 1);

        for (i = 0; i < indentation; ++ i)
        {
            fputc(' ', os);
        }

        RB_TREE_PRINT_NODE(os, node);
        fputc('\n', os);

        RB_TREE_NS(print_tree)(os, tree, node->right, indentation + 1);
    }
}
#endif // RB_TREE_PRINT_TREE_REQUIRED


#ifdef RB_TREE_FOREACH_REQUIRED

static void RB_TREE_NS(tree_foreach)(RB_TREE_NS(tree) * tree,
                                         void * context,
                                         void (* foreach_callback)(void * context, RB_TREE_NS(node) * node))
{
    const RB_TREE_NS(node) * leaf = &tree->leaf;
    RB_TREE_NS(node) * node = tree->root;
    bool handleLeftSubtree = true;

    for (;;)
    {
        if (handleLeftSubtree && (node->left != leaf))
        {
            node = node->left;
            continue;
        }

        foreach_callback(context, node);

        if (node->right != leaf)
        {
            node = node->right;
            handleLeftSubtree = true;
            continue;
        }

        /* go up */
        for (;;)
        {
            if (node->parent == NULL)
            {
                goto End;
            }

            if (node->parent->right == node)
            {
                node = node->parent;
                continue;
            }

            RB_TREE_ASSERT(node->parent->left == node);
            node = node->parent;
            handleLeftSubtree = false;
            break;
        }
    }

End:
    return;
}

#endif

#ifdef RB_TREE_CLEAR_REQUIRED

static void RB_TREE_NS(tree_clear)(RB_TREE_NS(tree) * tree)
{
    RB_TREE_NS(uninit_tree)(tree);
    RB_TREE_NS(init_tree)(tree);
}

#endif

/*
 * undefine fixed alloc namespace specifier
 */
#undef RB_TREE_FIXED_ALLOC_NS

/*
 * undefine user macros
 */

#undef RB_TREE_KEY_TYPE
#undef RB_TREE_XMALLOC
#undef RB_TREE_XREALLOC
#undef RB_TREE_XFREE
#undef RB_TREE_COMPARE
#undef RB_TREE_NS
#undef RB_TREE_FIND_NODE_REQUIRED
#undef RB_TREE_ADD_NODE_REQUIRED
#undef RB_TREE_REMOVE_NODE_REQUIRED
#undef RB_TREE_PRINT_TREE_REQUIRED
#undef RB_TREE_IS_VALID_TREE_REQUIRED
#undef RB_TREE_PRINT_NODE
#undef RB_TREE_INITIAL_CHUNK_SIZE
#undef RB_TREE_USER_DATA_TYPE
#undef RB_TREE_COUNT_REQUIRED
#undef RB_TREE_FOREACH_REQUIRED
#undef RB_TREE_CLEAR_REQUIRED
