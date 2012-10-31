
/*
 * template implementation of the fixed allocation mechanism.
 *
 * this file comes under the MIT license that described at
 * http://www.opensource.org/licenses/mit-license.php.
 *
 * the template instantiation is controlled by the following macro definitions:
 *
 * required macros:
 *  FIXED_ALLOC_ELEMENT_TYPE - defines element type
 *  FIXED_ALLOC_XMALLOC - defines memory allocation function, that will never return 0
 *  FIXED_ALLOC_XFREE - defines memory releasing function
 *
 * optional macros:
 *  FIXED_ALLOC_NS - namespace macro
 *  FIXED_ALLOC_FREE_FUNCTION_REQUIRED - if defined, allocator will be capable to free elements, free function will be available
 *  FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED - if defined, get status function will be made available
 *  FIXED_ALLOC_INITIAL_CHUNK_SIZE - defines initial chunk size in bytes for allocator without "free capabilities", in chunks for the other
 *  FIXED_ALLOC_CHUNK_NUM_BITS - only for allocator with "free-element" capabilities, defines bits count in an unsigned int number
 *  FIXED_ALLOC_FOREACH_REQUIRED - specifies that foreach function is required
 *  FIXED_ALLOC_CLEAR_REQUIRED - specifies that allocator_clear is required
 *  FIXED_ALLOC_ASSERT - specifies assertion
 *
 * unmasked types/functions:
 *  allocator                       allocator structure
 *  init_allocator                  initializes allocator
 *  uninit_allocator                uninitializes allocator
 *  alloc_elem                      allocates new element
 *  free_elem                       disposes element given that was previously allocated by this allocator
 *  get_allocator_status            retrieves allocator status
 *  allocator_foreach               enumerates allocated elements
 *  allocator_clear                 disposes all the allocated elements
 *
 *  internal_chunk                  internally used chunk structure
 *  internal_chunk_cluster          internally used chunk cluster structure
 *  internal_create_allocator       internal function
 *  internal_alloc_elem_from_chunk  internal function
 *  internal_find_free_elem         internal function
 *  internal_get_bits_count         internal function - get bits count from the number given, to be removed to the separate header
 *
 * Alexander Shabanov, 2008-2009
 * mailto:avshabanov@gmail.com
 * http://www.alexshabanov.com
 */

/*
 sample usage:

    // define type names
    #define FIXED_ALLOC_NS(n)            int_##n
    #define FIXED_ALLOC_ELEMENT_TYPE     int
    #define FIXED_ALLOC_XMALLOC          xmalloc
    #define FIXED_ALLOC_XFREE            xfree
    #define FIXED_ALLOC_INITIAL_CHUNK_SIZE (256)
    #define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
    #define FIXED_ALLOC_FREE_FUNCTION_REQUIRED

    #include <templates/fixed_alloc.h>

    ...
    int_allocator        intallocator;
    int * num;
    size_t used, allocated;

    int_init_allocator(&intallocator);

    num = int_alloc_elem(&intallocator);
    int_free_elem(&intallocator, num);
    
    int_get_allocator_status(&intallocator, &used, &allocated);
    
    int_uninit_allocator(&intallocator);
 */

#include <string.h>


/*
 * name specifier that originates the name
 */
#ifndef FIXED_ALLOC_NS
#define FIXED_ALLOC_NS(name) name
#endif

/*
 * imported types
 */

#ifndef FIXED_ALLOC_ELEMENT_TYPE
#error FIXED_ALLOC_ELEMENT_TYPE is not defined
#endif

/*
 * imported functions
 */

#ifndef FIXED_ALLOC_ASSERT
#include <assert.h>
#define FIXED_ALLOC_ASSERT(condition) assert(condition)
#endif

#ifndef FIXED_ALLOC_XMALLOC
#error FIXED_ALLOC_XMALLOC is not defined
#endif

#ifndef FIXED_ALLOC_XFREE
#error FIXED_ALLOC_XFREE is not defined
#endif

/*
 * initial cache size
 */

#ifndef FIXED_ALLOC_INITIAL_CHUNK_SIZE
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE   (1024)
#endif

#ifndef FIXED_ALLOC_FREE_FUNCTION_REQUIRED

/* free element function is not needed */

/*
 * allocator context type definition
 */
typedef struct FIXED_ALLOC_NS(internal_chunk)
{
    FIXED_ALLOC_ELEMENT_TYPE   arr[FIXED_ALLOC_INITIAL_CHUNK_SIZE];
    size_t               size;
    struct FIXED_ALLOC_NS(internal_chunk) * prev;
} FIXED_ALLOC_NS(internal_chunk);

typedef struct FIXED_ALLOC_NS(allocator)
{
    FIXED_ALLOC_NS(internal_chunk) * chunk;
} FIXED_ALLOC_NS(allocator);

static FIXED_ALLOC_NS(internal_chunk) *
FIXED_ALLOC_NS(internal_create_chunk)(FIXED_ALLOC_NS(internal_chunk) * prev)
{
    FIXED_ALLOC_NS(internal_chunk) * chunk = FIXED_ALLOC_XMALLOC(sizeof(FIXED_ALLOC_NS(internal_chunk)));
    chunk->size = 0;
    chunk->prev = prev;

    return chunk;
}

static void
FIXED_ALLOC_NS(init_allocator)(FIXED_ALLOC_NS(allocator) * allocator)
{
    allocator->chunk = NULL;
}

static void
FIXED_ALLOC_NS(uninit_allocator)(FIXED_ALLOC_NS(allocator) * allocator)
{
    FIXED_ALLOC_NS(internal_chunk) * chunk = allocator->chunk;
    while (NULL != chunk)
    {
        FIXED_ALLOC_NS(internal_chunk) * prev = chunk->prev;
        FIXED_ALLOC_XFREE(chunk);
        chunk = prev;
    }
}

static FIXED_ALLOC_ELEMENT_TYPE *
FIXED_ALLOC_NS(alloc_elem)(FIXED_ALLOC_NS(allocator) * allocator)
{
    FIXED_ALLOC_NS(internal_chunk) * chunk = allocator->chunk;

    if (chunk == 0 || chunk->size >= FIXED_ALLOC_INITIAL_CHUNK_SIZE)
    {
        chunk = FIXED_ALLOC_NS(internal_create_chunk)(chunk);
        allocator->chunk = chunk;
    }

    return &chunk->arr[chunk->size ++];
}

#ifdef FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
static void
FIXED_ALLOC_NS(get_allocator_status)(FIXED_ALLOC_NS(allocator) * allocator, size_t * used, size_t * allocated)
{
    FIXED_ALLOC_NS(internal_chunk) * c = allocator->chunk;

    *used = 0;
    *allocated = 0;

    while (NULL != c)
    {
        *used += c->size;
        *allocated += FIXED_ALLOC_INITIAL_CHUNK_SIZE;

        c = c->prev;
    }
}
#endif // FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED

#ifdef FIXED_ALLOC_FOREACH_REQUIRED

static void
FIXED_ALLOC_NS(allocator_foreach)(FIXED_ALLOC_NS(allocator) * allocator, void * context,
                                     void (* foreach_callback)(void * context, FIXED_ALLOC_ELEMENT_TYPE * e))
{
    FIXED_ALLOC_NS(internal_chunk) * c = allocator->chunk;

    while (NULL != c)
    {
        size_t i;

        for (i = 0; i < c->size; ++i)
        {
            foreach_callback(context, &c->arr[i]);
        }

        c = c->prev;
    }
}

#endif // FIXED_ALLOC_FOREACH_REQUIRED

#else // FIXED_ALLOC_FREE_FUNCTION_REQUIRED defined

#include <limits.h>

typedef unsigned long                   FIXED_ALLOC_NS(InternalMaskType);

#if !defined(FIXED_ALLOC_MASK_TYPE_MAX)

#define FIXED_ALLOC_MASK_TYPE_MAX    ULONG_MAX

#endif /* FIXED_ALLOC_MASK_TYPE_MAX */

#undef FIXED_ALLOC_CHUNK_NUM_BITS
#define FIXED_ALLOC_CHUNK_NUM_BITS   (sizeof(FIXED_ALLOC_NS(InternalMaskType)) * CHAR_BIT)

typedef struct FIXED_ALLOC_NS(internal_chunk)
{
    FIXED_ALLOC_NS(InternalMaskType)        free_mask;
    FIXED_ALLOC_ELEMENT_TYPE arr[FIXED_ALLOC_CHUNK_NUM_BITS];
} FIXED_ALLOC_NS(internal_chunk);



typedef struct FIXED_ALLOC_NS(internal_chunk_cluster)
{
    struct FIXED_ALLOC_NS(internal_chunk_cluster) *    prev;
    
    /*
     * heuristic parameter:
     * determines nearest free chunk's index in this block
     */
    size_t                                              nfc_index;

    FIXED_ALLOC_NS(internal_chunk)                    chunks[1];
} FIXED_ALLOC_NS(internal_chunk_cluster);

typedef struct FIXED_ALLOC_NS(allocator)
{
    FIXED_ALLOC_NS(internal_chunk_cluster) * cluster;
} FIXED_ALLOC_NS(allocator);

static void
FIXED_ALLOC_NS(init_allocator)(FIXED_ALLOC_NS(allocator) * allocator)
{
    allocator->cluster = NULL;
}

static void
FIXED_ALLOC_NS(uninit_allocator)(FIXED_ALLOC_NS(allocator) * allocator)
{
    FIXED_ALLOC_NS(internal_chunk_cluster) * c = allocator->cluster;
    while (NULL != c)
    {
        FIXED_ALLOC_NS(internal_chunk_cluster) * prev = c->prev;
        FIXED_ALLOC_XFREE(c);
        c = prev;
    }
}


static FIXED_ALLOC_ELEMENT_TYPE *
FIXED_ALLOC_NS(internal_alloc_elem_from_chunk)(FIXED_ALLOC_NS(internal_chunk) * chunk)
{
    size_t i;
    FIXED_ALLOC_NS(InternalMaskType) mask = 1;

    FIXED_ALLOC_ASSERT(chunk->free_mask != FIXED_ALLOC_MASK_TYPE_MAX);
    
    for (i = 0; i < FIXED_ALLOC_CHUNK_NUM_BITS; ++i)
    {
        if (0 == (chunk->free_mask & mask))
        {
            // mark place as busy
            chunk->free_mask |= mask;
            return &chunk->arr[i];
        }

        mask = mask << 1;
    }

    FIXED_ALLOC_ASSERT(!"should never come here");
    return 0;
}

static FIXED_ALLOC_ELEMENT_TYPE *
FIXED_ALLOC_NS(internal_find_free_elem)(FIXED_ALLOC_NS(internal_chunk_cluster) * cluster)
{
    for (; cluster != NULL; cluster = cluster->prev)
    {
        size_t i;
        
        for (i = cluster->nfc_index; i < FIXED_ALLOC_INITIAL_CHUNK_SIZE; ++ i)
        {
            FIXED_ALLOC_NS(internal_chunk) * chunk = &cluster->chunks[i];
            if (chunk->free_mask != FIXED_ALLOC_MASK_TYPE_MAX)
            {
                // free chunk has been found!
                FIXED_ALLOC_ELEMENT_TYPE * result = FIXED_ALLOC_NS(internal_alloc_elem_from_chunk)(chunk);
                
                // update `last free chunk index' for this block if
                // it pointed for this block before
                if (chunk->free_mask == FIXED_ALLOC_MASK_TYPE_MAX && cluster->nfc_index == i)
                {
                    cluster->nfc_index = i + 1;
                }

                return result;
            }
        }
    }

    return NULL;
}

static FIXED_ALLOC_ELEMENT_TYPE *
FIXED_ALLOC_NS(alloc_elem)(FIXED_ALLOC_NS(allocator) * allocator)
{
    FIXED_ALLOC_ELEMENT_TYPE * result;
    FIXED_ALLOC_NS(internal_chunk_cluster) * cluster = allocator->cluster;

    result = FIXED_ALLOC_NS(internal_find_free_elem)(cluster);
    if (NULL == result)
    {
        // need to allocate one another block (all the blocks busy)
        size_t s = sizeof(FIXED_ALLOC_NS(internal_chunk_cluster)) +
            (FIXED_ALLOC_INITIAL_CHUNK_SIZE - 1) * sizeof(FIXED_ALLOC_NS(internal_chunk));
        FIXED_ALLOC_NS(internal_chunk_cluster) * new_cluster = FIXED_ALLOC_XMALLOC(s);
        
        memset(new_cluster, 0, s);
        new_cluster->prev = cluster;
        allocator->cluster = new_cluster;

        // since new block is allocated - the following function must succeed
        result = FIXED_ALLOC_NS(internal_find_free_elem)(new_cluster);
        FIXED_ALLOC_ASSERT(result != 0);
    }

    return result;
}

/*
 * removes one element
 */
static void
FIXED_ALLOC_NS(free_elem)(FIXED_ALLOC_NS(allocator) * allocator, FIXED_ALLOC_ELEMENT_TYPE * elem)
{
    const void *    p = elem;
    FIXED_ALLOC_NS(internal_chunk_cluster) * cluster = allocator->cluster;
    FIXED_ALLOC_NS(internal_chunk) * chunk;
    size_t          chunk_index;
    size_t          arr_index;

    // find block the given list belongs to
    for (; cluster != NULL; cluster = cluster->prev)
    {
        void * left = cluster;
        void * right = (char *)cluster + sizeof(FIXED_ALLOC_NS(internal_chunk_cluster)) +
            (FIXED_ALLOC_INITIAL_CHUNK_SIZE - 1) * sizeof(FIXED_ALLOC_NS(internal_chunk));
        
        if ((p > left) && (p < right))
        {
            break;
        }
    }

    // block may not be null and list address shall not be less than the first one chunk entry
    FIXED_ALLOC_ASSERT((NULL != cluster) && (p >= (const void *)&cluster->chunks[0].arr[0]));

    // get chunk index
    chunk_index = (((size_t)p) - ((size_t)cluster->chunks)) / sizeof(FIXED_ALLOC_NS(internal_chunk));
    FIXED_ALLOC_ASSERT(chunk_index < FIXED_ALLOC_INITIAL_CHUNK_SIZE);

    chunk = &cluster->chunks[chunk_index];

    // now find offset in chunk arr
    arr_index = (size_t)p - (size_t)chunk->arr;
    FIXED_ALLOC_ASSERT(0 == (arr_index % sizeof(FIXED_ALLOC_ELEMENT_TYPE)));

    // calculate an exact index
    arr_index = arr_index / sizeof(FIXED_ALLOC_ELEMENT_TYPE);
    FIXED_ALLOC_ASSERT(arr_index < FIXED_ALLOC_CHUNK_NUM_BITS);

    // so at last index found - re-check that
    FIXED_ALLOC_ASSERT(chunk->arr + arr_index == elem);

    // check that element is not released twice
    FIXED_ALLOC_ASSERT((chunk->free_mask & (((FIXED_ALLOC_NS(InternalMaskType))1) << arr_index)) != 0);

    // mark this element as free
    chunk->free_mask &= ~(((FIXED_ALLOC_NS(InternalMaskType))1) << arr_index);

    // update last free chunk index if it is needed
    if (cluster->nfc_index > chunk_index)
    {
        cluster->nfc_index = chunk_index;
    }
}

#ifdef FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED

static size_t
FIXED_ALLOC_NS(internal_get_bits_count)(FIXED_ALLOC_NS(InternalMaskType) num)
{
    size_t  result = 0;
    size_t  i;
    for (i = 0; i < FIXED_ALLOC_CHUNK_NUM_BITS; ++ i)
    {
        result += (num & 1);
        num = num >> 1;
    }

    return result;
}

static void
FIXED_ALLOC_NS(get_allocator_status)(FIXED_ALLOC_NS(allocator) * allocator, size_t * used, size_t * allocated)
{
    FIXED_ALLOC_NS(internal_chunk_cluster) * cluster;

    *used = 0;
    *allocated = 0;

    for (cluster = allocator->cluster; NULL != cluster; cluster = cluster->prev)
    {
        int i;

        *allocated += FIXED_ALLOC_CHUNK_NUM_BITS * FIXED_ALLOC_INITIAL_CHUNK_SIZE;

        for (i = 0; i < FIXED_ALLOC_INITIAL_CHUNK_SIZE; ++ i)
        {
            *used += FIXED_ALLOC_NS(internal_get_bits_count)(cluster->chunks[i].free_mask);
        }
    }
}
#endif // FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED

#ifdef FIXED_ALLOC_FOREACH_REQUIRED

static void
FIXED_ALLOC_NS(allocator_foreach)(FIXED_ALLOC_NS(allocator) * allocator, void * context,
                                     void (* foreach_callback)(void * context, FIXED_ALLOC_ELEMENT_TYPE * e))
{
    FIXED_ALLOC_NS(internal_chunk_cluster) * cluster;

    for (cluster = allocator->cluster; NULL != cluster; cluster = cluster->prev)
    {
        size_t i;

        for (i = 0; i < FIXED_ALLOC_INITIAL_CHUNK_SIZE; ++ i)
        {
            FIXED_ALLOC_NS(internal_chunk) * chunk = &cluster->chunks[i];

            if (chunk->free_mask != 0)
            {
                FIXED_ALLOC_NS(InternalMaskType) mask = 1;
                size_t j;

                for (j = 0; j < FIXED_ALLOC_CHUNK_NUM_BITS; ++j)
                {
                    if (chunk->free_mask & mask)
                    {
                        foreach_callback(context, &chunk->arr[j]);
                    }

                    mask = mask << 1;
                }
            }
        }
    }
}

#endif // FIXED_ALLOC_FOREACH_REQUIRED

#endif // FIXED_ALLOC_FREE_FUNCTION_REQUIRED

#ifdef FIXED_ALLOC_CLEAR_REQUIRED

static void
FIXED_ALLOC_NS(allocator_clear)(FIXED_ALLOC_NS(allocator) * allocator)
{
    FIXED_ALLOC_NS(allocator_uninit)(allocator);
    FIXED_ALLOC_NS(allocator_init)(allocator);
}

#endif

/*
 * undefine user macros
 */
#undef FIXED_ALLOC_ELEMENT_TYPE
#undef FIXED_ALLOC_XMALLOC
#undef FIXED_ALLOC_XFREE
#undef FIXED_ALLOC_NS
#undef FIXED_ALLOC_FREE_FUNCTION_REQUIRED
#undef FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
#undef FIXED_ALLOC_INITIAL_CHUNK_SIZE
#undef FIXED_ALLOC_CHUNK_NUM_BITS
#undef FIXED_ALLOC_ASSERT
#undef FIXED_ALLOC_FOREACH_REQUIRED
#undef FIXED_ALLOC_CLEAR_REQUIRED
