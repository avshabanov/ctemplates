
#include <utilities/ut/ut.h>
#include <utilities/alloc.h>

/*
 * test stack 1
 */
#define STACK_NS(name)       int_##name
#define STACK_ELEMENT_TYPE   int
#define STACK_XREALLOC       xrealloc
#define STACK_XFREE          xfree
#define STACK_GROW_SIZE      (4)

#include <templates/stack.h>

static void stktst1()
{
    int_stack stack;
    UT_BEGIN("stack 1");

    int_stack_init(&stack);

    UT_VERIFY(int_stack_empty(&stack));
    UT_VERIFY((stack.elements == NULL) && (stack.allocated == 0) && (stack.size == 0));

    int_stack_uninit(&stack);
    UT_END();
}

static void stktst2()
{
    int_stack stack;
    int n;
    UT_BEGIN("stack 2");

    int_stack_init(&stack);

    int_stack_push(&stack, 12);
    UT_VERIFY((!int_stack_empty(&stack)) && (stack.size == 1) && (stack.allocated == 4));

    n = int_stack_pop(&stack);
    UT_VERIFY(int_stack_empty(&stack) && (stack.size == 0) && (stack.allocated == 4));

    int_stack_push(&stack, 11);
    UT_VERIFY((stack.size == 1) && (stack.allocated == 4));
    int_stack_push(&stack, 12);
    UT_VERIFY((stack.size == 2) && (stack.allocated == 4));
    int_stack_push(&stack, 13);
    UT_VERIFY((stack.size == 3) && (stack.allocated == 4));
    int_stack_push(&stack, 14);
    UT_VERIFY((stack.size == 4) && (stack.allocated == 4));

    int_stack_push(&stack, 15);
    UT_VERIFY((stack.size == 5) && (stack.allocated == 8));

    n = int_stack_pop(&stack);
    UT_VERIFY((n == 15) && (stack.size == 4) && (stack.allocated == 8));
    n = int_stack_pop(&stack);
    UT_VERIFY((n == 14) && (stack.size == 3));
    n = int_stack_pop(&stack);
    UT_VERIFY((n == 13) && (stack.size == 2));
    n = int_stack_pop(&stack);
    UT_VERIFY((n == 12) && (stack.size == 1));
    n = int_stack_pop(&stack);
    UT_VERIFY((n == 11) && (stack.size == 0) && int_stack_empty(&stack));

    int_stack_push(&stack, 123);
    UT_VERIFY((stack.size == 1) && (stack.allocated == 8));

    int_stack_uninit(&stack);
    UT_END();
}

/*
 * test stack with preallocated elements
 */
#define STACK_NS(name)       dbl_##name
#define STACK_ELEMENT_TYPE   double
#define STACK_XREALLOC       xrealloc
#define STACK_XFREE          xfree
#define STACK_PREALLOCATED_SIZE (2)
#define STACK_GROW_SIZE      (2)

#include <templates/stack.h>

static void stktst3()
{
    dbl_stack stack;
    double n;
    UT_BEGIN("stack 3: with preallocated elements");

    dbl_stack_init(&stack);
    UT_VERIFY(dbl_stack_empty(&stack) && (stack.size == 0) && (stack.allocated == 0));

    dbl_stack_push(&stack, 1.0);
    UT_VERIFY(!dbl_stack_empty(&stack) && (stack.size == 1) && (stack.allocated == 0));
    dbl_stack_push(&stack, 2.0);
    UT_VERIFY((stack.size == 2) && (stack.allocated == 0));
    
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 2.0) && (stack.size == 1) && (stack.allocated == 0));
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 1.0) && (stack.size == 0) && (stack.allocated == 0));

    dbl_stack_push(&stack, 1.0);
    dbl_stack_push(&stack, 2.0);
    UT_VERIFY((stack.size == 2) && (stack.allocated == 0));
    dbl_stack_push(&stack, 3.0);
    UT_VERIFY((stack.size == 3) && (stack.allocated == 2));
    dbl_stack_push(&stack, 4.0);
    UT_VERIFY((stack.size == 4) && (stack.allocated == 2));
    dbl_stack_push(&stack, 5.0);
    UT_VERIFY((stack.size == 5) && (stack.allocated == 4));

    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 5.0) && (stack.size == 4) && (stack.allocated == 4));
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 4.0) && (stack.size == 3) && (stack.allocated == 4));
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 3.0) && (stack.size == 2) && (stack.allocated == 4));
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 2.0) && (stack.size == 1) && (stack.allocated == 4));
    n = dbl_stack_pop(&stack);
    UT_VERIFY((n == 1.0) && (stack.size == 0) && (stack.allocated == 4));

    dbl_stack_uninit(&stack);
    UT_END();
}

/*
 * test pack
 */
void test_stack()
{
    stktst1();
    stktst2();
    stktst3();
}
