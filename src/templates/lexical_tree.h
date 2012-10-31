

/*
 * template implementation of the lexical tree data structure and operations on it.
 *
 * TODO:    this is an experimental data structure to fasten searching over the set of strings
 * WARNING: this template is not ready yet!
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  LEX_TREE_CHAR_TYPE - defines character type
 *  LEX_TREE_XMALLOC - defines memory allocation function, that never returns 0
 *  LEX_TREE_XREALLOC - defines memory reallocation function, that never returns 0
 *  LEX_TREE_XFREE - defines memory releasing function
 *
 * optional macros:
 *  LEX_TREE_NS - namespace macro
 *  LEX_TREE_NODE_VALUE_TYPE - specifies user-level value type attached to the node
 *  LEX_TREE_ENTRY_VALUE_TYPE - specifies user-level value type attached to the entry
 *  LEX_TREE_ENTRY_ARR_GROW_SIZE - entry array grow size
 *
 * unmasked types/functions:
 *  entry           entry structure
 *  node            node structure
 *  new_tree        allocates new tree
 *  free_tree       disposes previously allocated tree
 *  add_lexeme      adds new lexeme to the tree
 * TODO: complete
 *
 * Alexander Shabanov, 2010
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef LEX_TREE_NS
#define LEX_TREE_NS(name) name
#endif

#ifndef LEX_TREE_CHAR_TYPE
#error LEX_TREE_CHAR_TYPE is not defined
#endif

#ifndef LEX_TREE_XMALLOC
#error LEX_TREE_XMALLOC is not defined
#endif

#ifndef LEX_TREE_XREALLOC
#error LEX_TREE_XREALLOC is not defined
#endif

#ifndef LEX_TREE_XFREE
#error LEX_TREE_XFREE is not defined
#endif

#ifndef LEX_TREE_ENTRY_ARR_GROW_SIZE
#define LEX_TREE_ENTRY_ARR_GROW_SIZE (16)
#endif


/**
 * entry type
 */
typedef struct LEX_TREE_NS(entry)
{
    LEX_TREE_CHAR_TYPE          character;

    struct LEX_TREE_NS(node) *  node;

#ifdef LEX_TREE_ENTRY_VALUE_TYPE
    LEX_TREE_ENTRY_VALUE_TYPE   entry_value;
#endif
} LEX_TREE_NS(entry);


/**
 * node type
 */
typedef struct LEX_TREE_NS(node)
{
    struct LEX_TREE_NS(node) *  parent;

    int                         entry_allocated;
    int                         entry_count;
    LEX_TREE_NS(entry) *        entry_arr;

#ifdef LEX_TREE_NODE_VALUE_TYPE
    LEX_TREE_NODE_VALUE_TYPE    node_value;
#endif
} LEX_TREE_NS(node);

/**
 * tree type
 */
typedef struct
{
    LEX_TREE_NS(node) *  root;
} LEX_TREE_NS(tree);


/**
 * internal function
 * allocates new node
 */
static LEX_TREE_NS(node) *
LEX_TREE_NS(internal_new_node)(LEX_TREE_NS(node) * parent)
{
    LEX_TREE_NS(node) * result = LEX_TREE_XMALLOC(sizeof(LEX_TREE_NS(node)));
    memset(result, 0, sizeof(LEX_TREE_NS(node)));
    result->parent = parent;
    return result;
}

/**
 * internal function
 * recursively releases node and all it's subnodes as well as entries array
 */
static void
LEX_TREE_NS(internal_free_node)(LEX_TREE_NS(node) * n)
{
    int i;

    for (i = 0; i < n->entry_count; ++i)
    {
        LEX_TREE_NS(internal_free_node)(n->entry_arr[i].node);
    }

    LEX_TREE_XFREE(n->entry_arr);
    LEX_TREE_XFREE(n);
}

//
#define LEX_TREE_BSEARCH_NS(name) LEX_TREE_NS(internal_lexical_tree_##name)
#define BSEARCH_NS(name)        LEX_TREE_BSEARCH_NS(name)
#define BSEARCH_ARRAY_TYPE      LEX_TREE_NS(entry) *
#define BSEARCH_KEY_TYPE        LEX_TREE_CHAR_TYPE
// universal compare function based on real comparations rather than simple
// substraction because char type can be unsigned
#define BSEARCH_3W_COMPARE(array, index, key) \
    (key > array[index].character ? 1 : (key < array[index].character ? -1 : 0))

#include "bsearch.h"

/**
 * adds new lexeme to the tree
 */
static LEX_TREE_NS(node) *
LEX_TREE_NS(tree_add_lexeme)(LEX_TREE_NS(tree) * t, const LEX_TREE_CHAR_TYPE * lexeme, size_t len)
{
    LEX_TREE_NS(node) * cur_node = t->root;
    const LEX_TREE_CHAR_TYPE * lexeme_end = lexeme + len;

    for (; lexeme < lexeme_end; ++lexeme)
    {
        LEX_TREE_CHAR_TYPE character = *lexeme;
        int index = LEX_TREE_BSEARCH_NS(binary_search)(cur_node->entry_arr, cur_node->entry_count, character);
        struct LEX_TREE_NS(entry) * cur_entry;

        if (index < 0)
        {
            // insert new node at the index
            index = ~index;
            if (cur_node->entry_count == cur_node->entry_allocated)
            {
                // resize entries array
                cur_node->entry_allocated += LEX_TREE_ENTRY_ARR_GROW_SIZE;
                cur_node->entry_arr = LEX_TREE_XREALLOC(
                        cur_node->entry_arr,
                        cur_node->entry_allocated * sizeof(LEX_TREE_NS(entry)));
            }

            cur_entry = cur_node->entry_arr + index;

            memmove(cur_entry + 1, cur_entry, sizeof(LEX_TREE_NS(entry)) * (cur_node->entry_count - index));

            ++cur_node->entry_count;


            cur_entry->character = character;
            cur_entry->node = LEX_TREE_NS(internal_new_node)(cur_node);
        }
        else
        {
            cur_entry = cur_node->entry_arr + index;
        }

        cur_node = cur_entry->node;
    }

    return cur_node;
}


static void
LEX_TREE_NS(tree_init)(LEX_TREE_NS(tree) * t)
{
    t->root = LEX_TREE_NS(internal_new_node)(NULL);
}


static void
LEX_TREE_NS(tree_uninit)(LEX_TREE_NS(tree) * t)
{
    LEX_TREE_NS(internal_free_node)(t->root);
}



/**
 * retrieves lexeme to the buffer provided
 * it is expected that node points to the last one lexeme's element
 * \param node          last lexeme's node
 * \param buffer        destination buffer
 * \param buffer_size   destination buffer size
 * \returns size_t      length of the lexeme retrieved
 */
static size_t
LEX_TREE_NS(node_get_lexeme)(LEX_TREE_NS(node) * n, LEX_TREE_CHAR_TYPE * buffer, size_t buffer_size)
{
    LEX_TREE_CHAR_TYPE * last_iter = buffer + buffer_size;

    // get count
    LEX_TREE_NS(node) * prev_node = NULL;
    while (n != NULL && last_iter > buffer)
    {
        int i;

        for (i = 0; i < n->entry_count; ++i)
        {
            struct LEX_TREE_NS(entry) * e = n->entry_arr + i;
            if (e->node == prev_node)
            {
                *--last_iter = e->character;
                break;
            }
        }

        prev_node = n;
        n = n->parent;
    }

    // move string back to the left buffer bound
    size_t len = buffer + buffer_size - last_iter;
    memmove(buffer, last_iter, len);

    return len;
}


/**
 * internally used context for nodes enumeration
 */
typedef struct
{
    void (* buffer_ready_callback)(size_t lexeme_len, void * instance_data);

    void * instance_data;

    const LEX_TREE_CHAR_TYPE * lexeme_part_end;

    LEX_TREE_CHAR_TYPE * buffer_begin;

    const LEX_TREE_CHAR_TYPE * buffer_end;
} LEX_TREE_NS(internal_enum_context);

static void
LEX_TREE_NS(internal_enum_leaf_lexemes)(LEX_TREE_NS(node) * cur_node,
                                        const LEX_TREE_CHAR_TYPE * lexeme_part_begin,
                                        LEX_TREE_CHAR_TYPE * buffer,
                                        LEX_TREE_NS(internal_enum_context) * ctx)
{
    if (lexeme_part_begin < ctx->lexeme_part_end)
    {
        // we still have a part of lexeme
        LEX_TREE_CHAR_TYPE character = *lexeme_part_begin;

        int index = LEX_TREE_BSEARCH_NS(binary_search)(cur_node->entry_arr, cur_node->entry_count, character);
        if (index >= 0 && buffer < ctx->buffer_end)
        {
            *buffer = character;
            LEX_TREE_NS(internal_enum_leaf_lexemes)(cur_node->entry_arr[index].node,
                                                    lexeme_part_begin + 1,
                                                    buffer + 1,
                                                    ctx);
        }
    }
    else
    {
        // lexeme is over, but we still have a nodes

        if (cur_node->entry_count == 0)
        {
            // it's a leaf
            ctx->buffer_ready_callback(buffer - ctx->buffer_begin, ctx->instance_data);
        }
        else if (buffer < ctx->buffer_end)
        {
            // still have a nodes and a buffer to write to
            int i;

            for (i = 0; i < cur_node->entry_count; ++i)
            {
                struct LEX_TREE_NS(entry) * e = cur_node->entry_arr + i;
                *buffer = e->character;

                LEX_TREE_NS(internal_enum_leaf_lexemes)(e->node,
                                                        lexeme_part_begin,
                                                        buffer + 1,
                                                        ctx);
            }
        }
    }
}

/**
 * enumerates all the lexemes
 */
static inline void
LEX_TREE_NS(tree_enum_leaf_lexemes)(LEX_TREE_NS(tree) * t,
                                    const LEX_TREE_CHAR_TYPE * lexeme_part_begin,
                                    const LEX_TREE_CHAR_TYPE * lexeme_part_end,
                                    LEX_TREE_CHAR_TYPE * buffer,
                                    size_t buffer_size,
                                    void (* buffer_ready_callback)(size_t lexeme_len, void * instance_data),
                                    void * instance_data)
{
    LEX_TREE_NS(internal_enum_context) ctx;
    ctx.buffer_begin = buffer;
    ctx.buffer_end = buffer + buffer_size;
    ctx.buffer_ready_callback = buffer_ready_callback;
    ctx.instance_data = instance_data;
    ctx.lexeme_part_end = lexeme_part_end;

    LEX_TREE_NS(internal_enum_leaf_lexemes)(t->root, lexeme_part_begin, buffer, &ctx);
}

// undefine bsearch namespace macro
#undef LEX_TREE_BSEARCH_NS

// undefine user macros
#undef LEX_TREE_NS
#undef LEX_TREE_CHAR_TYPE
#undef LEX_TREE_XMALLOC
#undef LEX_TREE_XREALLOC
#undef LEX_TREE_XFREE
#undef LEX_TREE_ENTRY_ARR_GROW_SIZE
