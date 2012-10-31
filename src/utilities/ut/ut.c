
#include "ut.h"

int ut_tests_failed = 0;

int ut_tests_succeed = 0;

int ut_warnings = 0;

const char * ut_first_failed_test_file_name = NULL;
int ut_first_failed_test_file_line = -1;
const char * ut_first_failed_test_func_name = NULL;
const char * ut_first_failed_test_expr_name = NULL;
const char * ut_first_warning = NULL;

void ut_try_register_failed_test(const char * file_name, int line, const char * func_name, const char * expr)
{
    if (ut_first_failed_test_file_name == NULL)
    {
        ut_first_failed_test_file_name = file_name;
        ut_first_failed_test_file_line = line;
        ut_first_failed_test_func_name = func_name;
        ut_first_failed_test_expr_name = expr;
    }
}

void ut_try_register_warning(const char * warning)
{
    ++ut_warnings;
    if (ut_first_warning != NULL)
    {
        ut_first_warning = warning;
    }
}

void ut_final_report()
{
    fprintf(stderr, "\nfinal UT report:\n" 
            "\ttests failed:    %d\n"
            "\ttests succeed:   %d\n"
            "-----------------------------------\n"
            , ut_tests_failed, ut_tests_succeed);
    if (ut_warnings > 0)
    {
        fprintf(stderr,
            "\nWARNINGS: %d\nFirst warning is: %s\n",
            ut_warnings,
            ut_first_warning
            );
    }

    if (ut_tests_failed > 0)
    {
        fprintf(stderr,
            "\n\t!!! AT LEAST ONE TEST FAILED !!!\n\n"
            "First failed test case:\n"
            "in file %s\n"
            "at line %d in function %s\n"
            "in expression `%s' failed\n"
            , ut_first_failed_test_file_name
            , ut_first_failed_test_file_line
            , ut_first_failed_test_func_name
            , ut_first_failed_test_expr_name
            );
    }
    else
    {
        fputs("\n\t  888888   88  88   888", stderr);
        fputs("\n\t 88     8  88 88    888", stderr);
        fputs("\n\t 88     8  8888      88", stderr);
        fputs("\n\t 88     8  88 8      88", stderr);
        fputs("\n\t 88     8  88  8      8", stderr);
        fputs("\n\t 88     8  88   8      ", stderr);
        fputs("\n\t  888888   88    8    8", stderr);
        fputs("\n", stderr);
    }
}
