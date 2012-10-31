
#include <utilities/ut/ut.h>
#include <utilities/alloc.h>

/*
 * test stack without preallocated elements
 */
#define VECTOR_NS(name)       ch_##name
#define VECTOR_ELEMENT_TYPE   char
#define VECTOR_XREALLOC       xrealloc
#define VECTOR_XFREE          xfree
#define VECTOR_GROW_SIZE      (3)
#include <templates/vector.h>

static void test_char_vector()
{
    ch_vector vector;
    UT_BEGIN("vector: with chars");

    ch_vector_init(&vector);

    ch_vector_push_back(&vector, 'A');

    UT_VERIFY_CRITICAL((ch_vector_size(&vector) == 1) && (ch_vector_data(&vector)[0] == 'A'));

    ch_vector_uninit(&vector);
    UT_END();
}

/*
 * test stack with preallocated elements
 */
#define VECTOR_NS(name)       dbl_##name
#define VECTOR_ELEMENT_TYPE   double
#define VECTOR_XREALLOC       xrealloc
#define VECTOR_XFREE          xfree
#define VECTOR_PREALLOCATED_SIZE (2)
#define VECTOR_GROW_SIZE      (2)
#include <templates/vector.h>

static void test_double_vector()
{
    dbl_vector vector;
    //double n;
    UT_BEGIN("vector: with preallocated doubles");

    dbl_vector_init(&vector);

    dbl_vector_push_back(&vector, 40.0);

    UT_VERIFY_CRITICAL((dbl_vector_size(&vector) == 1) && (dbl_vector_data(&vector)[0] == 40.0));

    dbl_vector_uninit(&vector);
    UT_END();
}

/*
 * test pack
 */
void test_vector()
{
    test_char_vector();
    test_double_vector();
}
