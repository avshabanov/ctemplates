
#include <utilities/alloc.h>
#include <utilities/ut/ut.h>

// test cases entry points
void test_fixed_alloc();
void test_bsearch();
void test_stack();
void test_vector();
void test_avl_tree();
void test_rb_tree();
void test_lexical_tree();

int main()
{
    int result = 0;
    fprintf(stderr, "tests started\n");

    /* tests goes here */
    test_fixed_alloc();
    test_bsearch();
    test_vector();
    test_stack();
    test_avl_tree();
    test_rb_tree();
    test_lexical_tree();

    // end of tests
    UT_FINAL_REPORT();

    return result;
}
