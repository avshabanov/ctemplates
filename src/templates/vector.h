
/*
 * template implementation of the vector data structure and corresponding algorithms.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  VECTOR_ELEMENT_TYPE - defines element type
 *  VECTOR_XREALLOC - defines memory reallocation function, that never returns 0
 *  VECTOR_XFREE - defines memory releasing function
 *
 * optional macros:
 *  VECTOR_NS - namespace macro
 *  VECTOR_ASSERT - specifies user-level assert macro
 *  VECTOR_GROW_SIZE - specifies grow size constant that is used to recalculate reallocation size
 *  VECTOR_PREALLOCATED_SIZE - specifies that vector has preallocated part that
 *                             comes as a part of the vector structure what can result
 *                             in a valuable performance benefit when the vector's size is less than preallocated one
 *  VECTOR_CLEAR_REQUIRED - specifies whether vector_clear function is required
 *
 * unmasked types/functions:
 *  vector              vector structure
 *  vector_init         initializes vector
 *  vector_uninit       uninitializes vector
 *  vector_push_back    pushes back element to certain vector
 *  vector_data         returns pointer to the vector's data
 *  vector_size         returns size of the vector
 *  vector_clear        empties vector's contents
 *
 * Alexander Shabanov, 2008-2009
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

#include <stddef.h>
#include <stdbool.h>

#ifndef VECTOR_NS
#define VECTOR_NS(name) name
#endif

#ifndef VECTOR_ELEMENT_TYPE
#error VECTOR_ELEMENT_TYPE is not defined
#endif

#ifndef VECTOR_XREALLOC
#error VECTOR_XREALLOC is not been defined
#endif

#ifndef VECTOR_XFREE
#error VECTOR_XFREE is not been defined
#endif

#ifndef VECTOR_ASSERT
#include <assert.h>
#define VECTOR_ASSERT(x) assert(x)
#endif

#ifndef VECTOR_GROW_SIZE
#define VECTOR_GROW_SIZE  (256)
#endif

#ifdef VECTOR_PREALLOCATED_SIZE
/* for memcpy */
#include <string.h>
#endif


/**
 * defines vector structure
 */
typedef struct
{
#ifdef VECTOR_PREALLOCATED_SIZE
    VECTOR_ELEMENT_TYPE         prealloc_elements[VECTOR_PREALLOCATED_SIZE];
    bool                        prealloc_used;
#endif

    VECTOR_ELEMENT_TYPE *       elements;

    size_t                      allocated;
    size_t                      size;
} VECTOR_NS(vector);


/**
 * initializes the vector
 * \param vector    vector to be initialized
 */
static void VECTOR_NS(vector_init)(VECTOR_NS(vector) * vector)
{
#ifdef VECTOR_PREALLOCATED_SIZE
    vector->prealloc_used = true;
#endif
    vector->elements = NULL;
    vector->allocated = 0;
    vector->size = 0;
}

/**
 * uninitializes the vector
 * \param vector    vector to be uninitialized
 */
static void VECTOR_NS(vector_uninit)(VECTOR_NS(vector) * vector)
{
    VECTOR_XFREE(vector->elements);
}

/**
 * pushes the element provided back to the vector
 * \param vector        source vector
 * \param element       element to be pushed back
 */
static void VECTOR_NS(vector_push_back)(VECTOR_NS(vector) * vector, VECTOR_ELEMENT_TYPE element)
{
#ifdef VECTOR_PREALLOCATED_SIZE
    if (vector->prealloc_used)
    {
        VECTOR_ASSERT((NULL == vector->elements) && (0 == vector->allocated));

        if (vector->size < VECTOR_PREALLOCATED_SIZE)
        {
            /* element can be placed into the preallocated array */
            vector->prealloc_elements[vector->size++] = element;
            return;
        }

        /* preallocated array can not be used to store this element,
         * allocate new array and copy existing elements to it
         */
        vector->prealloc_used = false;
        vector->allocated = (VECTOR_PREALLOCATED_SIZE / VECTOR_GROW_SIZE + 1) * VECTOR_GROW_SIZE;

        VECTOR_ASSERT((vector->allocated > vector->size) && (VECTOR_PREALLOCATED_SIZE == vector->size));

        vector->elements = VECTOR_XREALLOC(0, vector->allocated * sizeof(VECTOR_ELEMENT_TYPE));

        memcpy(vector->elements, vector->prealloc_elements, VECTOR_PREALLOCATED_SIZE * sizeof(VECTOR_ELEMENT_TYPE));
    }
#endif

    if ((vector->size + 1) > vector->allocated)
    {
        vector->allocated += VECTOR_GROW_SIZE;
        vector->elements = VECTOR_XREALLOC(vector->elements, sizeof(VECTOR_ELEMENT_TYPE) * vector->allocated);
    }

    vector->elements[vector->size++] = element;
}

/**
 * returns a pointer to the vector's data
 * \param vector    source vector
 * \return pointer to the elements array
 */
static inline VECTOR_ELEMENT_TYPE * VECTOR_NS(vector_data)(VECTOR_NS(vector) * vector)
{
#ifdef VECTOR_PREALLOCATED_SIZE
    if (vector->prealloc_used)
    {
        return vector->prealloc_elements;
    }
#endif

    return vector->elements;
}

/**
 * returns size of the vector
 * \param vector    source vector
 * \return size of the vector
 */
static inline size_t VECTOR_NS(vector_size)(VECTOR_NS(vector) * vector)
{
    return vector->size;
}

/**
 * clears the vector's contents without deallocating the internal buffers
 * \param vector    source vector
 */
#ifdef VECTOR_CLEAR_REQUIRED
static void VECTOR_NS(vector_clear)(VECTOR_NS(vector) * vector)
{
    vector->size = 0;
}
#endif

/*
 * undefine user macros
 */
#undef VECTOR_ELEMENT_TYPE
#undef VECTOR_XREALLOC
#undef VECTOR_XFREE
#undef VECTOR_NS
#undef VECTOR_ASSERT
#undef VECTOR_GROW_SIZE
#undef VECTOR_PREALLOCATED_SIZE
#undef VECTOR_CLEAR_REQUIRED
