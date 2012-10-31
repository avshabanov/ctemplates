
#include <stdio.h>
#include <stdlib.h>

#include "utilities/ut/ut.h"
#include "utilities/alloc.h"


#define LEX_TREE_CHAR_TYPE  char
#define LEX_TREE_XMALLOC    xmalloc
#define LEX_TREE_XREALLOC   xrealloc
#define LEX_TREE_XFREE      xfree
#include <templates/lexical_tree.h>

#define CHAR_BUF_SIZE (32)

static node * tree_add_lexeme_len(tree * t, const char * lexeme)
{
    return tree_add_lexeme(t, lexeme, strlen(lexeme));
}

#define CMP_NODE_STR(cur_node, buf, buf_size, expected_str) \
{ \
    size_t len = node_get_lexeme(cur_node, buf, buf_size); \
    UT_VERIFY_CRITICAL(len < buf_size); \
    buf[len] = 0; \
    UT_VERIFY(expected_str && (0 == strcmp(buf, expected_str))); \
}

typedef struct
{
    char * buf;
    size_t lex_count;
} enum_context;

static void enum_callback(size_t lexeme_len, void * instance_data)
{
    enum_context * ctx = instance_data;
    ++ctx->lex_count;

    if (lexeme_len >= CHAR_BUF_SIZE)
    {
        fprintf(stderr, "too little buffer at %s:%d!\n", __FILE__, __LINE__);
        return;
    }

    ctx->buf[lexeme_len] = 0;
    fprintf(stdout, "%s\n", ctx->buf);
}

static void test_char_lex_tree()
{
    tree t;
    char buf[CHAR_BUF_SIZE];
    node * n1;
    node * n2;
    node * n3;
    node * n4;
    enum_context enum_ctx;

    UT_BEGIN("lexical tree test #1");

    enum_ctx.buf = buf;

    tree_init(&t);

    UT_VERIFY(t.root != NULL);

    tree_add_lexeme_len(&t, "XFER");
    n1 = tree_add_lexeme_len(&t, "ABCD");
    n2 = tree_add_lexeme_len(&t, "ACD");
    n3 = tree_add_lexeme_len(&t, "ABBA");
    n4 = tree_add_lexeme_len(&t, "BDA");

    CMP_NODE_STR(n1, buf, CHAR_BUF_SIZE, "ABCD");
    CMP_NODE_STR(n2, buf, CHAR_BUF_SIZE, "ACD");
    CMP_NODE_STR(n3, buf, CHAR_BUF_SIZE, "ABBA");
    CMP_NODE_STR(n4, buf, CHAR_BUF_SIZE, "BDA");

    enum_ctx.lex_count = 0;
    tree_enum_leaf_lexemes(&t, NULL, NULL, buf, CHAR_BUF_SIZE, &enum_callback, &enum_ctx);

    UT_VERIFY(enum_ctx.lex_count == 5);

    tree_uninit(&t);

    UT_END();
}


#define LEX_TREE_NS(name)   int_##name
#define LEX_TREE_CHAR_TYPE  int
#define LEX_TREE_XMALLOC    xmalloc
#define LEX_TREE_XREALLOC   xrealloc
#define LEX_TREE_XFREE      xfree
#include <templates/lexical_tree.h>

static void test_int_lex_tree()
{
    int_tree t;
    int lexeme[] = { 5, 9000, 71 };
    int_node * n;

    UT_BEGIN("lexical tree test #2");

    int_tree_init(&t);

    n = int_tree_add_lexeme(&t, lexeme, sizeof(lexeme) / sizeof(lexeme[0]));

    UT_VERIFY(0 == int_node_get_lexeme(n, NULL, 0));

    int_tree_uninit(&t);

    UT_END();
}

void test_lexical_tree()
{
    test_char_lex_tree();
    test_int_lex_tree();
}
