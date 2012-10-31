
/*
 * a small unit test framework
 *
 * A. Shabanov, 2009-2010
 */

#pragma once

#include <stdio.h>
#include <assert.h>



extern int ut_tests_failed;

extern int ut_tests_succeed;

extern int ut_warnings;


extern const char * ut_first_failed_test_file_name;
extern int ut_first_failed_test_file_line;
extern const char * ut_first_failed_test_func_name;
extern const char * ut_first_failed_test_expr_name;
extern const char * ut_first_warning;

extern void ut_try_register_failed_test(const char * file_name, int line, const char * func_name, const char * expr);

extern void ut_try_register_warning(const char * warning);

extern void ut_final_report();

#define UT_WARNING(message) ut_try_register_warning(message)

#define UT_BEGIN(ut_name)\
    const char * ut_current_name = ut_name;\
    int ut_loc_tests_failed = 0;\
    int ut_loc_tests_succeed = 0;\
    fprintf(stderr, "test case %s\n", ut_current_name)


#define UT_INTERNAL_FAILED(expr)\
    ++ut_loc_tests_failed;\
    ++ut_tests_failed;\
    ut_try_register_failed_test(__FILE__, __LINE__, __FUNCTION__, #expr)

#define UT_INTERNAL_SUCCEED(expr)\
    ++ut_loc_tests_succeed;\
    ++ut_tests_succeed


#define UT_END()\
    fprintf(stderr, "exiting %s\n\ttests failed:  %d\n\ttests succeed: %d\n====================\n",\
        ut_current_name,\
        ut_loc_tests_failed, ut_loc_tests_succeed);

#define UT_VERIFY(expr)\
    if (!(expr))\
    {\
        fprintf(stderr, "\t%s - failed\n", #expr);\
        UT_INTERNAL_FAILED(expr);\
    }\
    else\
    {\
        fprintf(stderr, "\t%s - succeed\n", #expr);\
        UT_INTERNAL_SUCCEED(expr);\
    }

#define UT_VERIFY_CRITICAL(expr)\
    if (!(expr))\
    {\
        fprintf(stderr, "\tCRITICAL %s - failed\n", #expr);\
        UT_INTERNAL_FAILED(expr);\
        UT_END();\
        return;\
    }\
    else\
    {\
        fprintf(stderr, "\t%s - succeed\n", #expr);\
        UT_INTERNAL_SUCCEED(expr);\
    }

#define UT_VERIFY_SILENT(expr)\
    if (!(expr))\
    {\
        fprintf(stderr, "\t%s - failed\n", #expr);\
        UT_INTERNAL_FAILED(expr);\
    }\
    else\
    {\
        UT_INTERNAL_SUCCEED(expr);\
    }

#define UT_FINAL_REPORT() ut_final_report()

