

#include <utilities/ut/ut.h>
#include <utilities/alloc.h>


#define BSEARCH_NS(name)     int_##name
#define BSEARCH_ARRAY_TYPE   int *
#define BSEARCH_KEY_TYPE     int
#define BSEARCH_3W_COMPARE(array, index, key) (key - array[index])
#include <templates/bsearch.h>

static void bsearch_test1()
{
    int arr[] = { 10, 20, 30, 40, 50 };

    UT_BEGIN("bsearch test #1");

    // positive tests
    UT_VERIFY(0 == int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 10));
    UT_VERIFY(1 == int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 20));
    UT_VERIFY(2 == int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 30));
    UT_VERIFY(3 == int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 40));
    UT_VERIFY(4 == int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 50));

    // negative tests
    UT_VERIFY(0 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 5));
    UT_VERIFY(1 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 15));
    UT_VERIFY(2 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 25));
    UT_VERIFY(3 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 35));
    UT_VERIFY(4 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 45));
    UT_VERIFY(5 == ~int_binary_search(arr, sizeof(arr) / sizeof(arr[0]), 55));


    UT_END();
}

void test_bsearch()
{
    bsearch_test1();
}
