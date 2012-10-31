
/*
 * template implementation of the stack data structure and operations on it.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  STACK_ELEMENT_TYPE - defines element type
 *  STACK_XREALLOC - defines memory reallocation function, that will returns 0
 *  STACK_XFREE - defines memory releasing function
 *
 * optional macros:
 *  STACK_NS - namespace macro
 *  STACK_ASSERT - specifies user-level assert macro
 *  STACK_GROW_SIZE - specifies grow size constant that is used to recalculate reallocation size
 *  STACK_PREALLOCATED_SIZE - specifies that stack has preallocated part that
 *                               comes as a part of the stack structure
 *                               this can result in a benefit when using stack as a local variable
 *
 * unmasked types/functions:
 *  stack           stack structure
 *  stack_init      initializes stack
 *  stack_uninit    uninitializes stack
 *  stack_push      pushes the given element back to the specified stack
 *  stack_pop       pops last element from the stack
 *  stack_empty     returns true if stack is empty, false otherwise
 *
 * Alexander Shabanov, 2008-2009
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

#include <stddef.h>
#include <stdbool.h>

#ifndef STACK_NS
#define STACK_NS(name) name
#endif

#ifndef STACK_ELEMENT_TYPE
#error STACK_ELEMENT_TYPE is not defined
#endif

#ifndef STACK_XREALLOC
#error STACK_XREALLOC is not been defined
#endif

#ifndef STACK_XFREE
#error STACK_XFREE is not been defined
#endif

#ifndef STACK_ASSERT
#include <assert.h>
#define STACK_ASSERT(x) assert(x)
#endif

#ifndef STACK_GROW_SIZE
#define STACK_GROW_SIZE  (256)
#endif

/**
 * stack data structure
 */
typedef struct STACK_NS(stack)
{
#ifdef STACK_PREALLOCATED_SIZE
    STACK_ELEMENT_TYPE      prealloc_elements[STACK_PREALLOCATED_SIZE];
#endif

    STACK_ELEMENT_TYPE *    elements;

    size_t                  allocated;
    size_t                  size;
} STACK_NS(stack);

/**
 * initializes the given stack structure
 */
static void STACK_NS(stack_init)(STACK_NS(stack) * stack)
{
    stack->elements = NULL;
    stack->allocated = 0;
    stack->size = 0;
}

/**
 * uninitializes the given stack structure
 */
static void STACK_NS(stack_uninit)(STACK_NS(stack) * stack)
{
    STACK_XFREE(stack->elements);
}

/**
 * pushes the given element to the stack
 * \param stack     destination stack
 * \param element   source element
 */
static void STACK_NS(stack_push)(STACK_NS(stack) * stack, STACK_ELEMENT_TYPE element)
{
#ifdef STACK_PREALLOCATED_SIZE
    if (stack->size < STACK_PREALLOCATED_SIZE)
    {
        /* element can be placed into the preallocated stack */
        stack->prealloc_elements[stack->size] = element;
    }
    else
    {
        /* place element into the allocated array */
        size_t alloc_size = stack->size - STACK_PREALLOCATED_SIZE;

        if ((alloc_size + 1) > stack->allocated)
        {
            stack->allocated += STACK_GROW_SIZE;
            stack->elements = STACK_XREALLOC(stack->elements, sizeof(STACK_ELEMENT_TYPE) * stack->allocated);
        }

        stack->elements[alloc_size] = element;
    }

    ++stack->size;
#else
    if ((stack->size + 1) > stack->allocated)
    {
        stack->allocated += STACK_GROW_SIZE;
        stack->elements = STACK_XREALLOC(stack->elements, sizeof(STACK_ELEMENT_TYPE) * stack->allocated);
    }

    stack->elements[stack->size++] = element;
#endif
}

/**
 * pops the last element from the stack
 * this function shall not be called if the stack is empty
 * \param stack     source stack
 * \return popped element
 */
static STACK_ELEMENT_TYPE STACK_NS(stack_pop)(STACK_NS(stack) * stack)
{
    STACK_ELEMENT_TYPE element;
    STACK_ASSERT(stack->size > 0);

#ifdef STACK_PREALLOCATED_SIZE
    --stack->size;

    if (stack->size < STACK_PREALLOCATED_SIZE)
    {
        element = stack->prealloc_elements[stack->size];
    }
    else
    {
        element = stack->elements[stack->size - STACK_PREALLOCATED_SIZE];
    }
#else
    element = stack->elements[--stack->size];
#endif

    return element;
}

/**
 * returns true if the given stack is empty
 */
static inline bool STACK_NS(stack_empty)(STACK_NS(stack) * stack)
{
    return (stack->size == 0);
}

/*
 * undefine user macros
 */
#undef STACK_ELEMENT_TYPE
#undef STACK_XREALLOC
#undef STACK_XFREE
#undef STACK_NS
#undef STACK_ASSERT
#undef STACK_GROW_SIZE
#undef STACK_PREALLOCATED_SIZE
