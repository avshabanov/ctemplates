
#include <utilities/ut/ut.h>
#include <utilities/alloc.h>

/*
 * test fixed allocator that is not capable to free elements allocated
 */

#define FIXED_ALLOC_NS(n)            int_##n
#define FIXED_ALLOC_ELEMENT_TYPE     int
#define FIXED_ALLOC_XMALLOC          xmalloc
#define FIXED_ALLOC_XFREE            xfree
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE (4)
#define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
#define FIXED_ALLOC_FOREACH_REQUIRED

#include <templates/fixed_alloc.h>

#define NS(name) int_##name
#define ELEMENT_TYPE int
#include "test_alloc_foreach.h"


static void fxtst1()
{
    int_allocator allocator;
    int * num;
    int * num1;
    int * num2;
    size_t used, allocated;

    UT_BEGIN("allocator w/o free function");

    int_init_allocator(&allocator);

    int_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 0) && (allocated == 0));

    UT_VERIFY(num = int_alloc_elem(&allocator));
    *num = -1;
    int_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 1) && (allocated == 4)); /* 4 is initial chunk size */

    num1 = int_alloc_elem(&allocator);
    *num1 = 0xFEEDFEED;
    int_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 2) && (allocated == 4));

    UT_VERIFY(num != num1);
    UT_VERIFY(num2 = int_alloc_elem(&allocator));
    *num2 = 0xAEF0AEF1;
    UT_VERIFY((num2 != num1) && (num != num2));

    int_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 3) && (allocated == 4));

    num = int_alloc_elem(&allocator);
    int_get_allocator_status(&allocator, &used, &allocated);
    *num = -4;
    UT_VERIFY((used == 4) && (allocated == 4));

    num = int_alloc_elem(&allocator);
    *num = 295;
    int_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 5) && (allocated == 8));

    {
        int arr[] = { -1, 0xFEEDFEED, 0xAEF0AEF1, -4, 295 };
        int_test_foreach("foreach test for non-free allocator", &allocator, arr, sizeof(arr)/sizeof(arr[0]));
    }

    int_uninit_allocator(&allocator);
    UT_END();
}

/*
 * test fixed allocator that is capable to free underlying elements
 */

#define FIXED_ALLOC_NS(n)            dbl_##n
#define FIXED_ALLOC_ELEMENT_TYPE     double
#define FIXED_ALLOC_XMALLOC          xmalloc
#define FIXED_ALLOC_XFREE            xfree
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE (3)
#define FIXED_ALLOC_FREE_FUNCTION_REQUIRED
#define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
#define FIXED_ALLOC_FOREACH_REQUIRED

#include <templates/fixed_alloc.h>

#define NS(name) dbl_##name
#define ELEMENT_TYPE double
#include "test_alloc_foreach.h"

static double get_next_num(size_t index)
{
    return (double)((index & 1) ? -(double)index : index + 1000.0);
}

static void fxtst2()
{
    dbl_allocator allocator;
    double * num1;
    double * num2;
    double * num3;
    double a;
    size_t alloc1;
    size_t next_num = 0;
    size_t used, allocated;
    UT_BEGIN("allocator w/free function");

    dbl_init_allocator(&allocator);

    dbl_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 0) && (allocated == 0));

    num1 = dbl_alloc_elem(&allocator);
    *num1 = 9.9e10;

    dbl_get_allocator_status(&allocator, &used, &allocated);
    alloc1 = allocated;
    UT_VERIFY((used == 1) && ((allocated % 4) == 0));

    dbl_free_elem(&allocator, num1);
    dbl_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 0) && (allocated == alloc1));

    num2 = dbl_alloc_elem(&allocator);
    a = get_next_num(next_num++);
    *num2 = a;
    UT_VERIFY(num1 == num2);

    dbl_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 1) && (allocated == alloc1));

    num3 = dbl_alloc_elem(&allocator);
    *num3 = get_next_num(next_num++);
    UT_VERIFY((num3 != num2) && (*num2 == a));

    num1 = dbl_alloc_elem(&allocator);
    a = 2.3e14;
    *num1 = a;
    UT_VERIFY((*num3 != *num1) && (*num1 == a));

    dbl_get_allocator_status(&allocator, &used, &allocated);
    UT_VERIFY((used == 3) && (allocated == alloc1));

    

    // allocate 3 * alloc1 elements - this must multiply elements
    {
        size_t i;
        const size_t alloc_series = 3;
        size_t cnt = alloc1 * alloc_series;
        size_t expected_used_size = cnt + used;
        size_t expected_alloc_size = cnt + alloc1;

        a = *num3;

        for (i = 0; i < cnt; ++i)
        {
            double b;
            num2 = dbl_alloc_elem(&allocator);
            b = get_next_num(next_num++);
            *num2 = b;
            UT_VERIFY_SILENT(((num3 < num2) && (num3 + 1 <= num2)) || ((num3 > num2) && (num2 + 1 <= num3)));
            UT_VERIFY_SILENT((*num2 == b) && (*num3 == a));

            a = b;
            num3 = num2;
        }

        dbl_get_allocator_status(&allocator, &used, &allocated);
        UT_VERIFY((used == expected_used_size) && (allocated == expected_alloc_size));

        dbl_free_elem(&allocator, num1);
        dbl_get_allocator_status(&allocator, &used, &allocated);
        UT_VERIFY((used == (expected_used_size - 1)) && (allocated == expected_alloc_size));

        num2 = dbl_alloc_elem(&allocator);
        *num2 = get_next_num(next_num++);
        dbl_get_allocator_status(&allocator, &used, &allocated);
        UT_VERIFY((used == expected_used_size) && (used == next_num) && (allocated == expected_alloc_size));
    }

    {
        double * arr = xmalloc(sizeof(double)*next_num);
        size_t i;
        for (i = 0; i < next_num; ++i)
        {
            arr[i] = get_next_num(i);
        }

        dbl_test_foreach("foreach test for free allocator", &allocator, arr, next_num);

        xfree(arr);
    }

#if 0
    // this must produce an assertion that tells that element released twice
    dbl_free_elem(&allocator, num1);
#endif

    dbl_uninit_allocator(&allocator);
    UT_END();
}

/*
 * test fixed alloc on large amounts of data w/multiple alloc/free scenarios
 */

struct MyStruct
{
    int x;
    unsigned short y;
    double z;
};


#define MYS_INIT(s, ax, ay, az)\
    {\
        (s)->x = (ax);\
        (s)->y = (ay);\
        (s)->z = (az);\
    }

#define MYS_INIT2(from, to)\
    {\
        (from)->x = (to)->x;\
        (from)->y = (to)->x;\
        (from)->z = (to)->x;\
    }

#define MYS_EQUALS(s1, s2) (((s1)->x == (s2)->x) && ((s1)->y == (s2)->y) && ((s1)->z == (s2)->z))

#define MYS_EQUALS2(s1, ax, ay, az) (((s1)->x == (ax)) && ((s1)->y == (ay)) && ((s1)->z == (az)))

#define FIXED_ALLOC_NS(n)            mys_##n
#define FIXED_ALLOC_ELEMENT_TYPE     struct MyStruct
#define FIXED_ALLOC_XMALLOC          xmalloc
#define FIXED_ALLOC_XFREE            xfree
#define FIXED_ALLOC_INITIAL_CHUNK_SIZE (2)
#define FIXED_ALLOC_FREE_FUNCTION_REQUIRED
#define FIXED_ALLOC_GET_ALLOCATOR_STATUS_REQUIRED
//#define FIXED_ALLOC_FOREACH_REQUIRED

static void on_error(const char * what)
{
    fprintf(stderr, "Failed: %s\n", what);
}

#define FIXED_ALLOC_ASSERT(x)\
    if (!(x))\
    {\
        on_error(#x);\
        assert(!"fixed alloc assertion failed");\
    }

#include <templates/fixed_alloc.h>

#if 0
struct MysForeachContext
{
    struct MyStruct ** pointers;
    struct MyStruct * elements;
    size_t size;
    size_t index;
};

static void my_foreach_callback(void * context, struct MyStruct * e)
{
    struct MysForeachContext * mysf = context;
    assert(mysf->index < mysf->size);

    mysf->pointers[mysf->index] = e;
    MYS_INIT2(&mysf->elements[mysf->index], e);

    ++mysf->index;
}

static void init_mysf(struct MysForeachContext * mysf)
{
    /* fill elements */
    mysf->pointers = xmalloc(size * sizeof(struct MyStruct *));
    mysf->elements = xmalloc(size * sizeof(struct MyStruct));
    mysf->size = size;
    mysf->index = 0;

    mys_allocator_foreach(allocator, mysf, &my_foreach_callback);
}

static void uninit_mysf(struct MysForeachContext * mysf)
{
    xfree(mysf->pointers);
    xfree(mysf->elements);
}

static void alloc_free_seq(mys_allocator * allocator, size_t to_be_deleted, size_t to_be_added)
{
    size_t i;
    size_t size;
    size_t allocated;
    struct MysForeachContext mysf;
    struct MysForeachContext mysf2;

    UT_BEGIN("alloc free seq");

    mys_get_allocator_status(allocator, &size, &allocated);
    UT_VERIFY_CRITICAL(to_be_deleted > size);
    
    init_mysf(&mysf);
    UT_VERIFY_CRITICAL(mysf.index == mysf.size);

    for (i = 0; i < to_be_deleted; ++i)
    {
        mys_free_elem(allocator, mysf.pointers[i]);
    }

    mys_get_allocator_status(allocator, &size, &allocated);
    UT_VERIFY(size == (size - to_be_deleted));

    init_mysf(&mysf2);

    uninit_mysf(&mysf);
    uninit_mysf(&mysf2);
    UT_END();
}
#endif

static void fxtst3()
{
    mys_allocator allocator;
    size_t size;
    size_t allocated;
    struct MyStruct * e1;
    struct MyStruct * e2;
    size_t i;

    UT_BEGIN("fixed alloc w/multi alloc/free data scenario");
    mys_init_allocator(&allocator);

    e1 = mys_alloc_elem(&allocator);
    MYS_INIT(e1, 100001, 1, 10001.0001);

    e2 = mys_alloc_elem(&allocator);
    MYS_INIT(e2, 100002, 2, 10002.0002);

    UT_VERIFY(MYS_EQUALS2(e1, 100001, 1, 10001.0001));

    mys_get_allocator_status(&allocator, &size, &allocated);
    UT_VERIFY((size == 2) && (allocated > size));

    {
        struct MyStruct ** pp;
        
        const size_t len = allocated * 2 + 1;
        const size_t expected_size = len + size;
        const size_t expected_allocated = allocated * 3;
        size_t del_len;

        pp = xmalloc(sizeof(struct MyStruct *) * len);

        for (i = 0; i < len; ++i)
        {
            int k = i + 3;
            e1 = mys_alloc_elem(&allocator);
            UT_VERIFY_SILENT(e2 != e1);

            MYS_INIT(e1, k + 100000, (unsigned short)k, 100000 + (double)k + (double)k / 10000.0);

            pp[i] = e1;
            e2 = e1;
        }

        mys_get_allocator_status(&allocator, &size, &allocated);
        UT_VERIFY((size == expected_size) && (allocated == expected_allocated));

        del_len = len / 2 + 3;
        for (i = 0; i < del_len; ++i)
        {
            mys_free_elem(&allocator, pp[i]);
        }

        xfree(pp);
    }

    mys_uninit_allocator(&allocator);
    UT_END();
}

/*
 * function that launches tests
 */
void test_fixed_alloc()
{
    fxtst1();
    fxtst2();
    fxtst3();
}

